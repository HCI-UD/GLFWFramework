#include "CommonKinect.h"
#include "Common.h"

/* **********************************************************************************
#  																					#
# Copyright (c) 2015-2016,															#
# Chair for Computer Aided Medical Procedures & Augmented Reality (CAMPAR) I-16		#
# Technische Universität München													#
# 																					#
# All rights reserved.																#
# Felix Bork - felix.bork@tum.de													#
# 																					#
# Redistribution and use in source and binary forms, with or without				#
# modification, are restricted to the following conditions:							#
# 																					#
#  * The software is permitted to be used internally only by CAMPAR and				#
#    any associated/collaborating groups and/or individuals.						#
#  * The software is provided for your internal use only and you may				#
#    not sell, rent, lease or sublicense the software to any other entity			#
#    without specific prior written permission.										#
#    You acknowledge that the software in source form remains a confidential		#
#    trade secret of CAMPAR and therefore you agree not to attempt to				#
#    reverse-engineer, decompile, disassemble, or otherwise develop source			#
#    code for the software or knowingly allow others to do so.						#
#  * Redistributions of source code must retain the above copyright notice,			#
#    this list of conditions and the following disclaimer.							#
#  * Redistributions in binary form must reproduce the above copyright notice,		#
#    this list of conditions and the following disclaimer in the documentation		#
#    and/or other materials provided with the distribution.							#
#  * Neither the name of CAMPAR nor the names of its contributors may be used		#
#    to endorse or promote products derived from this software without				#
#    specific prior written permission.												#
# 																					#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND	#
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED		#
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE			#
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR	#
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES	#
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;		#
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND		#
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT		#
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS		#
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.						#
# 																					#
*************************************************************************************/

enum ZoomArea { Head, SpineBaseMid };

class BodyRegionZoomApp : public GLFWApp
{
public:

	/*
	Finds x y Position (center) of the given ZoomArea
	Take the CameraSpacePoint of the Joint and map it to a 2D ColorSpacePoint
	*/
	void virtual findXYPosition(ZoomArea id) {
		JointType joint;
		zoomArea = id;
		std::shared_ptr<Body> m_closestBody = m_kinectDataManger.GetPrimaryUser();
		if (m_closestBody != nullptr) {
			switch (id) {
			case Head: joint = JointType_Head; break;
			case SpineBaseMid: joint = JointType_SpineMid; break;
			default: return;
			}

			//Map to 2D ColorSpacePoint
			CameraSpacePoint cameraPoint(m_closestBody->GetJointMap().find(joint)->second.GetWorldPosition());
			ICoordinateMapper* coordinateMapper = m_kinectDataManger.GetCoordinateMapper();
			ColorSpacePoint* point = new ColorSpacePoint();
			coordinateMapper->MapCameraPointsToColorSpace(1, &cameraPoint, 1, point);
			zoomPositionX = point->X;
			zoomPositionY = point->Y;
		}

		zoomed = true;
		initializeP_zoomed();
	}

	/*
	Zoom the given picture 
	to a picture with the same size
	*/
	cv::Mat zoom(cv::Mat picture) {

		// column, row - new dimensions for zoomed picture (half the size)
		int column, row;
		column = picture.cols / 2;
		row = picture.rows / 2;

		//smallMat - new Matrix with half the size of the original picture
		cv::Mat smallMat;
		smallMat.rows = row;
		smallMat.cols = column;

		//x0, y0 - left (x) / upper (y) start of zooming area
		double x0 = zoomPositionX - column / 2;
		double y0 = zoomPositionY - row / 2;

		if (x0 < 0)
			x0 = 0;
		if (y0 < 0)
			y0 = 0;

		if (x0 >(column))
			x0 = column;
		if (y0 >(row))
			y0 = row;

		//Copy region of interest
		picture(cv::Rect(x0, y0, column, row)).copyTo(smallMat);
		
		//scale to right dimensions
		cv::Mat roi;
		roi.rows = picture.rows;
		roi.cols = picture.cols;
		cv::resize(smallMat, roi, roi.size(), 0, 0, cv::INTER_CUBIC);

		return roi;
	}

	/*
	Initialize the projectionMatrix for the zoomed picture
	Take a new matrix half the size, located in the center and move it according to the zoomPosition
	*/
	glm::mat4 initializeP_zoomed() {

		int column, row;
		column = m_windowDimensions.x;
		row = m_windowDimensions.y;
		int offsetX = column / 2 - zoomPositionX;
		int offsetY = row / 2 - zoomPositionY;

		double x0 = zoomPositionX - column / 4;
		double y0 = zoomPositionY - row / 4;


		if (x0 < 0)
			offsetX = column / 4;
		if (y0 < 0)
			offsetY = row / 4;

		if (x0 >(column / 2))
			offsetX = -column / 4;
		if (y0 >(row / 2))
			offsetY = -row / 4;

		float N = 1.0;
		float F = 10000.0;
		float Lzoomed;
		float Rzoomed;
		float Bzoomed;
		float Tzoomed;

		if (splitscreenEnabled) {
			Lzoomed = static_cast<float>(m_windowDimensions.x / 4 + m_windowDimensions.x / 8 - offsetX);
			Rzoomed = static_cast<float>(m_windowDimensions.x / 2 + m_windowDimensions.x / 8 - offsetX);
			Tzoomed = static_cast<float>(m_windowDimensions.y / 4 - offsetY);
			Bzoomed = static_cast<float>(m_windowDimensions.y - m_windowDimensions.y / 4 - offsetY);
		}
		else {
			Lzoomed = static_cast<float>(m_windowDimensions.x / 4 - offsetX);
			Rzoomed = static_cast<float>(m_windowDimensions.x - m_windowDimensions.x / 4 - offsetX);
			Bzoomed = static_cast<float>(m_windowDimensions.y - m_windowDimensions.y / 4 - offsetY);
			Tzoomed = static_cast<float>(m_windowDimensions.y / 4 - offsetY);

		}

		glm::mat4 matNDC_zoomed = glm::transpose(glm::mat4(2.0 / (Rzoomed - Lzoomed), 0.0, 0.0, -(Rzoomed + Lzoomed) / (Rzoomed - Lzoomed),
			0.0, 2.0 / (Tzoomed - Bzoomed), 0.0, -(Tzoomed + Bzoomed) / (Tzoomed - Bzoomed),
			0.0, 0.0, -2.0 / (F - N), -(F + N) / (F - N),
			0.0, 0.0, 0.0, 1.0));
		P_zoomed = matNDC_zoomed * matPerspective;

		return matNDC_zoomed * matPerspective;

	}

	// GLFW callbacks
	auto virtual OnKey(int key, int scancode, int action, int mods) -> void override
	{
		if (action != GLFW_PRESS)
		{
			return;
		}

		switch (key)
		{
		case GLFW_KEY_ESCAPE:
		{
			glfwSetWindowShouldClose(m_window, 1);
			break;
		}

		case GLFW_KEY_S:
		{
			splitscreenEnabled = !splitscreenEnabled;

			break;
		}

		case GLFW_KEY_0:
		{
			zoomed = false;
			break;
		}

		case GLFW_KEY_1:
		{
			zoomed = true;
			zoomArea = Head;
			break;
		}
		case GLFW_KEY_2:
		{
			zoomed = true;
			zoomArea = SpineBaseMid;
			break;
		}

		}
	}

	// GLFW window loop functions
	auto virtual Setup() -> void override
	{
		std::shared_ptr<DeviceOptions> deviceOptions = std::make_shared<DeviceOptions>();
		deviceOptions->SetIsColorEnabled(true);
		deviceOptions->SetIsInfraredEnabled(false);
		deviceOptions->SetIsLongExposureInfraredEnabled(false);
		deviceOptions->SetIsDepthEnabled(false);
		deviceOptions->SetIsBodyIndexEnabled(false);
		deviceOptions->SetIsBodyEnabled(true);
		deviceOptions->SetIsFaceEnabled(false);
		deviceOptions->SetIsHDFaceEnabled(true);

		HRESULT hr = m_kinectDataManger.InitializeKinectSensor(deviceOptions);
		if (FAILED(hr))
		{
			std::string error{ "Error while trying to initialize Kinect sensor!" };
			std::cerr << error << std::endl;
			throw std::runtime_error(error.c_str());
		}

		m_windowDimensions.x = m_kinectDataManger.GetColorFrame().cols;
		m_windowDimensions.y = m_kinectDataManger.GetColorFrame().rows;
	}


	auto virtual InitGL() -> void override
	{
		// OpenGL state
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		// Projection Matrix
		//
		// For AR applications, the projection matrix is composed of two different matrices, namely
		// the perspective matrix and the NDC matrix. The first one transforms to clip coordinates
		// while the seconds one transforms to normalized device coordinates. To calculate both of
		// these matrices, all clipping planes (L(eft), R(ight), T(op), B(ottom), N(ear) and F(ar))
		// have to be known.
		//
		// References:
		// 1) http://ksimek.github.io/2013/06/03/calibrated_cameras_in_opengl/
		// 2) http://www.songho.ca/opengl/gl_projectionmatrix.html

		float L = 0;
		float R = static_cast<float>(m_windowDimensions.x);
		float B = static_cast<float>(m_windowDimensions.y);
		float T = 0;
		float N = 1.0;
		float F = 10000.0;

		matPerspective = glm::transpose(glm::mat4(f_x, 0.0, -x_0, 0.0,
			0.0, f_y, -y_0, 0.0,
			0.0, 0.0, N + F, N * F,
			0.0, 0.0, -1.0, 0.0));

		glm::mat4 matNDC = glm::transpose(glm::mat4(2.0 / (R - L), 0.0, 0.0, -(R + L) / (R - L),  // Equivalent to glm::ortho(left, right, bottom, top, near, far)
			0.0, 2.0 / (T - B), 0.0, -(T + B) / (T - B),
			0.0, 0.0, -2.0 / (F - N), -(F + N) / (F - N),
			0.0, 0.0, 0.0, 1.0));

		L = static_cast<float>(m_windowDimensions.x / 4);
		R = static_cast<float>(m_windowDimensions.x - m_windowDimensions.x / 4);

		glm::mat4 matNDC_splitscreen = glm::transpose(glm::mat4(2.0 / (R - L), 0.0, 0.0, -(R + L) / (R - L),
			0.0, 2.0 / (T - B), 0.0, -(T + B) / (T - B),
			0.0, 0.0, -2.0 / (F - N), -(F + N) / (F - N),
			0.0, 0.0, 0.0, 1.0));

		P = matNDC * matPerspective;
		P_splitscreen = matNDC_splitscreen * matPerspective;

		// View Matrix
		//
		// In AR applications, the view matrix corresponds to the inverse of the extrinsic matrix.
		// However, in the view matrix from the calibration files from Meng is already inversed.
		// Due to the fact that OpenGL and OpenCV coordinate systems differ(one is y - up, one is 
		// y - down),  the view matrix has to be rotated PI radians around the x-axis. Because of 
		// the rotation, the z-axis  also rotates, so for all translations the z-value has to be
		// positive instead of negative as it should be usually.

		V = E;
		V = glm::rotate(glm::mat4(), static_cast<float>(3.14159265358979323846264338328), glm::vec3(1.0f, 0.0f, 0.0f)) * V;

		// UBO Matrices (binding index 0)
		ubo_matrices = GLUtils::BufferUtils::CreateBufferObject(GL_UNIFORM_BUFFER, nullptr, 2 * sizeof(glm::mat4), GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_matrices);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(V));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(P));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// UBO Light (binding index 1)
		glm::vec4 La = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec4 Ld = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec4 Ls = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec4 Le = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec4 LightPosition = glm::vec4(0.0f, 100.0f, 0.0f, 1.0f);
		ubo_lightInfo = GLUtils::BufferUtils::CreateBufferObject(GL_UNIFORM_BUFFER, nullptr, 5 * sizeof(glm::vec4), GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_lightInfo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_lightInfo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), glm::value_ptr(La));
		glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(Ld));
		glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(Ls));
		glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(Le));
		glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(LightPosition));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// UBO Material (binding index 2)
		glm::vec4 Ka = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
		glm::vec4 Kd = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
		glm::vec4 Ks = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
		glm::vec4 Ke = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
		float Shininess = 50.0f;
		ubo_materialInfo = GLUtils::BufferUtils::CreateBufferObject(GL_UNIFORM_BUFFER, nullptr, 4 * sizeof(glm::vec4) + 1 * sizeof(float), GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubo_materialInfo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_materialInfo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), glm::value_ptr(Ka));
		glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(Kd));
		glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(Ks));
		glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(Ke));
		glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4), sizeof(float), &Shininess);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Pointers
		skull = std::make_shared<PLYMesh>("../../resources/meshes/skull_split.ply");
		mandible = std::make_shared<PLYMesh>("../../resources/meshes/mandible_split.ply");
		camera = std::make_shared<Camera>();
		backgroundQuad = std::make_shared<BackgroundQuad>();

		// Textures
		tex_background = std::make_shared<Texture>(GL_TEXTURE_2D, nullptr, m_windowDimensions.x, m_windowDimensions.y);
		tex_splitscreenBackground = std::make_shared<Texture>(GL_TEXTURE_2D, nullptr, m_windowDimensions.x / 2, m_windowDimensions.y);

		// Framebuffer Objects
		fbo_arView = std::make_shared<FramebufferObject>();
		fbo_arView->AttachTexture2D(tex_background->GetHandle());
		fbo_arView->AttachRenderbuffer(FramebufferObject::RenderBufferAttachement::DEPTH, GL_DEPTH_COMPONENT, m_windowDimensions.x, m_windowDimensions.y);
		fbo_arView->Validate();

		// Programs
		prg_imageQuad = std::make_shared<ShaderProgram>("../../resources/shaders/TEXTURE.vs", "../../resources/shaders/TEXTURE.fs");
		prg_lighting = std::make_shared<ShaderProgram>("../../resources/shaders/Lighting.vs", "../../resources/shaders/Lighting.fs");
	}

	auto virtual Update() -> void override
	{

		if (m_kinectDataManger.GetDeviceOptions()->IsColorEnabled())
		{
			HRESULT hr = m_kinectDataManger.Update();
			if (SUCCEEDED(hr))
			{
				cv::Mat colorFrame = m_kinectDataManger.GetColorFrame();

				//Zooming - zoom the image
				if (zoomed) {
					findXYPosition(zoomArea);
					zoom(colorFrame).copyTo(colorFrame);
				}

				if (m_kinectDataManger.GetDeviceOptions()->IsBodyEnabled())
				{
					std::vector<std::shared_ptr<Body>> bodies = m_kinectDataManger.GetBodies();

					// Retrive primary user
					if (m_kinectDataManger.GetDeviceOptions()->IsHDFaceEnabled())
					{
						std::shared_ptr<Body> primaryUser = m_kinectDataManger.GetPrimaryUser();

						if (primaryUser != nullptr)
						{
							closestBodyHDFace = primaryUser->GetBodyHDFace();
						}
					}
				}

				tex_background->UpdateTexture(colorFrame.data, 0, colorFrame.cols, 0, colorFrame.rows);

				// In splitscreen mode, both the AR and VR view should take up half of the screen.
				// On both the left and right hand side of the Kinect color stream texture, one 
				// fourth are omitted in AR splitscreen mode. To achieve this, extract the center
				// of the color frame and create a new OpenCV matrix with that (clone is mandatory).
				// Then update the splitscreen texture and upload the data to the shader.
				if (splitscreenEnabled)
				{
					cv::Mat colorFrameSplitScreen = colorFrame(cv::Range::all(), cv::Range(colorFrame.cols / 4, colorFrame.cols - colorFrame.cols / 4));
					cv::Mat B = colorFrameSplitScreen.clone();
					tex_splitscreenBackground->UpdateTexture(B.data, 0, colorFrameSplitScreen.cols, 0, colorFrameSplitScreen.rows);
				}
			}
		}
	}

	auto virtual Draw() -> void override
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		int windowWidth, windowHeight;
		glfwGetWindowSize(m_window, &windowWidth, &windowHeight);

		/*
		* Check whether splitscreen mode is enabled.
		*
		* If splitscreen is NOT enabled, the regular AR view is
		* drawn, with a virtual skull and mandible overlayed on
		* top of the full Kinect color image.
		*
		* If splitscreen IS enabled, the left and right quarter
		* of the Kinect color stream are "cut" away, and only the
		* central half is rendered on the left half of the screen.
		* On the right half, a VR view containing only the virtual
		* models of the skull and mandible is displayed.
		*/

		if (!splitscreenEnabled)
		{
			/*
			* 1. Render Kinect color stream to the full screen.
			*
			* Viewport is set to full window dimensions and view and
			* projection matrices are set to identity.
			*/
			glViewport(0, 0, windowWidth, windowHeight);

			glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			prg_imageQuad->UseProgram();
			prg_imageQuad->SetUniform("modelMatrix", glm::mat4());
			glActiveTexture(GL_TEXTURE0);
			tex_background->Bind();
			backgroundQuad->Render();
			tex_background->Unbind();
			prg_imageQuad->UnUseProgram();

			/*
			* 2. Render virtual skull on top of head of closest user
			*
			* Keep viewport set to FULL window dimensions. Set view
			* matrix to extrinsic camera calibration matrix of the
			* Kinect, and the projection matrix based on the Kinect
			* instrinsic camera calibration matrix and a FULLSCREEN
			* viewport. Finally, draw the skull and mandible on top
			* of closest user.
			*/
			if (closestBodyHDFace != nullptr)
			{
				//Zooming
				if (zoomed) {
					glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
					glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(V));
					glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(P_zoomed));
					glBindBuffer(GL_UNIFORM_BUFFER, 0);
				}
				else {
					glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
					glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(V));
					glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(P));
					glBindBuffer(GL_UNIFORM_BUFFER, 0);
				}

				glEnable(GL_DEPTH_TEST);
				prg_lighting->UseProgram();
				// Draw skull
				skull->ResetModelMatrix();
				skull->ScaleModel(glm::vec3(200.0f, 200.0f, 200.0f));
				skull->RotateModel(glm::radians(static_cast<float>(closestBodyHDFace->GetFaceRollInDegrees())), glm::vec3(0.0f, 0.0f, 1.0f));
				skull->RotateModel(glm::radians(static_cast<float>(closestBodyHDFace->GetFaceYawInDegrees())), glm::vec3(0.0f, 1.0f, 0.0f));
				skull->RotateModel(glm::radians(static_cast<float>(closestBodyHDFace->GetFacePitchInDegrees())), glm::vec3(1.0f, 0.0f, 0.0f));
				skull->TranslateModel(1000.0f * glm::vec3(closestBodyHDFace->GetCentroidWorldSpace().X, closestBodyHDFace->GetCentroidWorldSpace().Y, closestBodyHDFace->GetCentroidWorldSpace().Z));
				prg_lighting->SetUniform("modelMatrix", skull->GetModelMatrix());
				skull->Render(GL_TRIANGLES);

				// Draw mandible
				mandible->ResetModelMatrix();
				mandible->ScaleModel(glm::vec3(200.0f, 200.0f, 200.0f));
				mandible->RotateModel(glm::radians(static_cast<float>(closestBodyHDFace->GetFaceRollInDegrees())), glm::vec3(0.0f, 0.0f, 1.0f));
				mandible->RotateModel(glm::radians(static_cast<float>(closestBodyHDFace->GetFaceYawInDegrees())), glm::vec3(0.0f, 1.0f, 0.0f));
				mandible->RotateModel(glm::radians(static_cast<float>(closestBodyHDFace->GetFacePitchInDegrees())), glm::vec3(1.0f, 0.0f, 0.0f));
				mandible->RotateModel(glm::radians(static_cast<float>(-closestBodyHDFace->GetMouthOpeningInCentimeters() * 7.5f)), glm::vec3(1.0f, 0.0f, 0.0f));
				mandible->TranslateModel(1000.0f * glm::vec3(closestBodyHDFace->GetCentroidWorldSpace().X, closestBodyHDFace->GetCentroidWorldSpace().Y, closestBodyHDFace->GetCentroidWorldSpace().Z));
				prg_lighting->SetUniform("modelMatrix", mandible->GetModelMatrix());
				mandible->Render(GL_TRIANGLES);
				prg_lighting->UnUseProgram();
				glDisable(GL_DEPTH_TEST);
			}
		}
		else
		{
			/*
			* 1. Render central half of the Kinect color stream to the left half of the screen.
			*
			* Viewport in x-direction is set to half window dimensions
			* and view and projection matrices are set to identity.
			*/
			glViewport(0, 0, windowWidth / 2, windowHeight);

			glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			prg_imageQuad->UseProgram();
			prg_imageQuad->SetUniform("modelMatrix", glm::mat4());
			glActiveTexture(GL_TEXTURE0);
			tex_splitscreenBackground->Bind();
			backgroundQuad->Render();
			tex_splitscreenBackground->Unbind();
			prg_imageQuad->UnUseProgram();

			/*
			* 2. Render virtual skull on top of head of closest user
			*
			* Keep viewport set to HALF window dimensions. Set view
			* matrix to extrinsic camera calibration matrix of the
			* Kinect, and the projection matrix based on the Kinect
			* instrinsic camera calibration matrix and a SPLITSCREEN
			* viewport. Finally, draw the skull and mandible on top
			* of closest user.
			*/
			if (closestBodyHDFace != nullptr)
			{
				//Zooming
				if (zoomed) {
					glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
					glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(V));
					glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(P_zoomed));
					glBindBuffer(GL_UNIFORM_BUFFER, 0);
				}
				else {
					glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
					glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(V));
					glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(P_splitscreen));
					glBindBuffer(GL_UNIFORM_BUFFER, 0);
				}

				glEnable(GL_DEPTH_TEST);
				prg_lighting->UseProgram();
				// Draw skull
				skull->ResetModelMatrix();
				skull->ScaleModel(glm::vec3(200.0f, 200.0f, 200.0f));
				skull->RotateModel(glm::radians(static_cast<float>(closestBodyHDFace->GetFaceRollInDegrees())), glm::vec3(0.0f, 0.0f, 1.0f));
				skull->RotateModel(glm::radians(static_cast<float>(closestBodyHDFace->GetFaceYawInDegrees())), glm::vec3(0.0f, 1.0f, 0.0f));
				skull->RotateModel(glm::radians(static_cast<float>(closestBodyHDFace->GetFacePitchInDegrees())), glm::vec3(1.0f, 0.0f, 0.0f));
				skull->TranslateModel(1000.0f * glm::vec3(closestBodyHDFace->GetCentroidWorldSpace().X, closestBodyHDFace->GetCentroidWorldSpace().Y, closestBodyHDFace->GetCentroidWorldSpace().Z));
				prg_lighting->SetUniform("modelMatrix", skull->GetModelMatrix());
				skull->Render(GL_TRIANGLES);

				// Draw mandible
				mandible->ResetModelMatrix();
				mandible->ScaleModel(glm::vec3(200.0f, 200.0f, 200.0f));
				mandible->RotateModel(glm::radians(static_cast<float>(closestBodyHDFace->GetFaceRollInDegrees())), glm::vec3(0.0f, 0.0f, 1.0f));
				mandible->RotateModel(glm::radians(static_cast<float>(closestBodyHDFace->GetFaceYawInDegrees())), glm::vec3(0.0f, 1.0f, 0.0f));
				mandible->RotateModel(glm::radians(static_cast<float>(closestBodyHDFace->GetFacePitchInDegrees())), glm::vec3(1.0f, 0.0f, 0.0f));
				mandible->RotateModel(glm::radians(static_cast<float>(-closestBodyHDFace->GetMouthOpeningInCentimeters() * 7.5f)), glm::vec3(1.0f, 0.0f, 0.0f));
				mandible->TranslateModel(1000.0f * glm::vec3(closestBodyHDFace->GetCentroidWorldSpace().X, closestBodyHDFace->GetCentroidWorldSpace().Y, closestBodyHDFace->GetCentroidWorldSpace().Z));
				prg_lighting->SetUniform("modelMatrix", mandible->GetModelMatrix());
				mandible->Render(GL_TRIANGLES);
				prg_lighting->UnUseProgram();
				glDisable(GL_DEPTH_TEST);
			}

			/*
			* 3. Render VR view on right half of the screen
			*
			* Set viewport to the other half in x-direction.
			* View and projection matrix are set based on the
			* OpenGL camera class. Finally, the skull and man-
			* dible are drawn to reflect the users motions.
			*/
			if (closestBodyHDFace != nullptr)
			{
				glViewport(windowWidth / 2, 0, windowWidth / 2, windowHeight);

				camera->SetAspectRatio(1.0f);

				glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->GetViewMatrix()));
				glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera->GetProjectionMatrix()));
				glBindBuffer(GL_UNIFORM_BUFFER, 0);

				glEnable(GL_DEPTH_TEST);
				prg_lighting->UseProgram();
				// Draw skull
				skull->ResetModelMatrix();
				skull->RotateModel(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				skull->RotateModel(glm::radians(static_cast<float>(-closestBodyHDFace->GetFaceRollInDegrees())), glm::vec3(0.0f, 0.0f, 1.0f));
				skull->RotateModel(glm::radians(static_cast<float>(-closestBodyHDFace->GetFaceYawInDegrees())), glm::vec3(0.0f, 1.0f, 0.0f));
				skull->RotateModel(glm::radians(static_cast<float>(closestBodyHDFace->GetFacePitchInDegrees())), glm::vec3(1.0f, 0.0f, 0.0f));
				prg_lighting->SetUniform("modelMatrix", skull->GetModelMatrix());
				skull->Render(GL_TRIANGLES);

				// Draw mandible
				mandible->ResetModelMatrix();
				mandible->RotateModel(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				mandible->RotateModel(glm::radians(static_cast<float>(-closestBodyHDFace->GetFaceRollInDegrees())), glm::vec3(0.0f, 0.0f, 1.0f));
				mandible->RotateModel(glm::radians(static_cast<float>(-closestBodyHDFace->GetFaceYawInDegrees())), glm::vec3(0.0f, 1.0f, 0.0f));
				mandible->RotateModel(glm::radians(static_cast<float>(closestBodyHDFace->GetFacePitchInDegrees())), glm::vec3(1.0f, 0.0f, 0.0f));
				mandible->RotateModel(glm::radians(static_cast<float>(-closestBodyHDFace->GetMouthOpeningInCentimeters() * 7.5f)), glm::vec3(1.0f, 0.0f, 0.0f));
				prg_lighting->SetUniform("modelMatrix", mandible->GetModelMatrix());
				mandible->Render(GL_TRIANGLES);
				prg_lighting->UnUseProgram();
				glDisable(GL_DEPTH_TEST);
			}
		}
	}

	auto virtual ShutdownGL() -> void override
	{
		GLUtils::BufferUtils::DeleteBufferObject(&ubo_matrices);
		GLUtils::BufferUtils::DeleteBufferObject(&ubo_lightInfo);
		GLUtils::BufferUtils::DeleteBufferObject(&ubo_materialInfo);
	}

	auto virtual Cleanup() -> void override
	{
		HRESULT hr = m_kinectDataManger.CloseKinectSensor();
		if (FAILED(hr))
		{
			std::string error{ "Error while trying to close Kinect sensor!" };
			std::cerr << error << std::endl;
			throw std::runtime_error(error.c_str());
		}
	}

private:
	KinectDataManager& m_kinectDataManger{ KinectDataManager::GetInstance() };

	std::shared_ptr<Body::BodyHDFace> closestBodyHDFace{ nullptr };

	GLuint ubo_matrices{ 0 };
	GLuint ubo_lightInfo{ 0 };
	GLuint ubo_materialInfo{ 0 };

	std::shared_ptr<PLYMesh> skull{ nullptr };
	std::shared_ptr<PLYMesh> mandible{ nullptr };
	std::shared_ptr<Camera> camera{ nullptr };
	std::shared_ptr<BackgroundQuad> backgroundQuad{ nullptr };

	std::shared_ptr<Texture> tex_background{ nullptr };
	std::shared_ptr<Texture> tex_splitscreenBackground{ nullptr };

	std::shared_ptr<FramebufferObject> fbo_arView{ nullptr };

	std::shared_ptr<ShaderProgram> prg_imageQuad{ nullptr };
	std::shared_ptr<ShaderProgram> prg_lighting{ nullptr };

	bool splitscreenEnabled{ false };

	glm::mat4 P{ glm::mat4() };
	glm::mat4 P_splitscreen{ glm::mat4() };
	glm::mat4 V{ glm::mat4() };

	// Kinect 1 Calibration Parameters

	double f_x{ 1.0555595295248311e+003 };
	double f_y{ 1.0553695516063312e+003 };
	double x_0{ 9.6845279782509590e+002 };
	double y_0{ 5.2657689367910041e+002 };
	glm::mat4 E{ glm::transpose(glm::mat4(9.9996929470031348e-001, 6.0588270175632817e-003,  4.9699367932315398e-003,  5.1917725950575111e+001,
		-6.0572078150210925e-003, 9.9998159687953281e-001, -3.4078694968998495e-004, -3.7189925174652749e-001,
		-4.9719101000640373e-003, 3.1067254574039988e-004,  9.9998759171928042e-001, -2.1870288086129372e+000,
		0.0,                     0.0,                     0.0,                       1.0)) };
	std::vector<double> distortionParameters{ { 4.2452333236524954e-002, -4.2231307253447571e-002, 9.6967086533845590e-004, 3.7297236935803001e-004, -1.1455528267188553e-003 } };
	double projectionError{ 2.5731037797761691e-001 };

	//Zooming
	glm::mat4 matPerspective;
	glm::mat4 P_zoomed{ glm::mat4() };
	double zoomPositionX;
	double zoomPositionY;
	bool zoomed = false;
	ZoomArea zoomArea;

};

int main(int argc, char ** argv)
{
	try
	{
		return BodyRegionZoomApp().Run();
	}
	catch (std::exception& error)
	{
		std::cerr << error.what() << std::endl;
	}
	catch (const std::string& error)
	{
		std::cerr << error.c_str() << std::endl;
	}

	return -1;
}