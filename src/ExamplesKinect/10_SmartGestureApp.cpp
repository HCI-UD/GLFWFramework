#include "asio.hpp"

#include "CommonKinect.h"
#include "Common.h"
#include "CommonSmartGesture.h"

#include <thread>
#include <functional>
#include <opencv2/highgui/highgui.hpp>
#include <fstream>
#include <list>
#include <ctime>
#include <math.h>
#include "gui\Button.h"

#define DEBUG 1
#define SHOWFPS 0

class SmartGestureApp : public GLFWApp, public AsyncUDPServer
{
public:

	SmartGestureApp(asio::io_service& io_service) :
		AsyncUDPServer(io_service)
	{
		do_receive();
	}

	auto virtual handle_receive() -> void override
	{
#if MEASURETIME
		std::cout << "time since last update: " << (std::clock() - timer2) / (double)CLOCKS_PER_SEC << std::endl;;
#endif
		qrDetected = true;
		char *udpMsg = getUDPMsg();
		//enter calibration mode, if prompt
		if (strcmp(udpMsg, "calibration") == 0) {
			doCalibration = true;
			splitscreenEnabled = false;
		}
		else if (strcmp(udpMsg, "continue") == 0) {
			doCalibration = false;
			resetTimer = std::clock();
		}
		else if (strcmp(udpMsg, "clickdown") == 0) {
			double lastClick = (std::clock() - clickDelay) / (double)CLOCKS_PER_SEC;
			if (lastClick > 1) {
				mouseClicked = true;
			}
			clickDelay = std::clock();

		}
		else {//normal mode for receiving coordinate points
			const char *delim = ";";
			char *next_token;
			char* pch = strtok_s(udpMsg, delim, &next_token);

			m_mousePosition.x = atoi(pch);
			pch = strtok_s(NULL, delim, &next_token);
			m_mousePosition.y = atoi(pch);
			SetCursorPos(m_mousePosition.x, m_mousePosition.y);
		}
		resetTimer = clock();

		do_receive();

	}

	auto virtual OnMouseMove(double x_, double y_) -> void override
	{
		int x = static_cast<int>(x_);
		int y = static_cast<int>(y_);

		int whichScreen = splitscreenEnabled ? (x > m_cfg->screenRes_Width / 2 ? B3D_FROMRIGHT : B3D_FROMLEFT) : B3D_FROMFULL;

		if (useOrgan) {
			std::string id = m_organButton->isInside(x, y, whichScreen);
			m_organ->setSelectionByParent(id);
		}
		

		m_GUIButton->isInside(x, y, whichScreen);
		
	}

	auto virtual OnMouseButton(int button, int action, int mods) -> void override
	{
		if (action == GLFW_PRESS) {
			mouseClicked = true;
		}

	}

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
			switchSplitscreen();
			break;
		}

		case GLFW_KEY_R:
		{
			m_bodies.clear();
			m_closestBody = nullptr;
			break;
		}

		case GLFW_KEY_A:
		{
			this->useOrgan = !this->useOrgan;
			if (useOrgan) {
				camera->SetAspectRatio(1.0f);
				camera->SetPosition(glm::vec3(0, -1.25f, 15.0f));
			}
			else {
				camera->SetAspectRatio(1.0f);
				camera->SetPosition(glm::vec3(0, 0.0f, 15.0f));
			}
			break;
		}

		case GLFW_KEY_X:
		{
			isMirror = !isMirror;
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
		deviceOptions->SetIsHDFaceEnabled(false);

		hr = m_kinectDataManger.InitializeKinectSensor(deviceOptions);
		if (FAILED(hr))
		{
			std::string error{ "Error while trying to initialize Kinect sensor!" };
			std::cerr << error << std::endl;
			throw std::runtime_error(error.c_str());
		}

		m_windowDimensions.x = m_kinectDataManger.GetColorFrame().cols;
		m_windowDimensions.y = m_kinectDataManger.GetColorFrame().rows;
		m_colorFrame = cv::Mat::zeros(m_windowDimensions.x, m_windowDimensions.y, CV_8UC4);

		//Create ConfigHandler and read the configuration file
		m_cfg = std::make_shared<ConfigHandler>();
	}

	void initSmartGesture() {

		//glfwHideWindow(m_window);
		m_organ = std::make_shared<OrganModel>("../../resources/meshes/organs/organs.obj");
		m_digSys = std::make_shared<OrganModel>("../../resources/meshes/digSystem/digSystem.obj");
		//glfwShowWindow(m_window);

		
		m_cfg->initScreen(m_window);
		m_cursorUser = std::make_shared<CursorHistory>(5, m_cfg);
		m_fingertip = std::make_shared<CursorHistory>(5, m_cfg);

		std::cout << "Loading GUI elements..."; //################################################################

		m_organButton = std::make_shared<ButtonsVR>(m_cfg->screenRes_Width, m_cfg->screenRes_Height);
		m_GUIButton = std::make_shared<ButtonsVR>(m_cfg->screenRes_Width, m_cfg->screenRes_Height);

		m_organButton->addButton("../../resources/meshes/button3D/button3D.obj","lungs.jpg", "lungs", glm::vec2(5, 0),[this]() {
			std::cout << "lung button got pressed" << std::endl;
		}, B3D_FULLSPLITRIGHT);

		m_organButton->addButton("../../resources/meshes/button3D/button3D.obj", "heart.jpg", "heart", glm::vec2(5, 1), [this]() {
			std::cout << "heart button got pressed" << std::endl;
		}, B3D_FULLSPLITRIGHT);

		m_organButton->addButton("../../resources/meshes/button3D/button3D.obj", "stomach.jpg", "stomach", glm::vec2(5, 2), [this]() {
			std::cout << "stomach button got pressed" << std::endl;
		}, B3D_FULLSPLITRIGHT);

		m_organButton->addButton("../../resources/meshes/button3D/button3D.obj", "liver.jpg", "liver", glm::vec2(5, 3), [this]() {
			std::cout << "liver button got pressed" << std::endl;
		}, B3D_FULLSPLITRIGHT);

		m_organButton->addButton("../../resources/meshes/button3D/button3D.obj", "bowel.jpg", "bowel", glm::vec2(5, 4), [this]() {
			std::cout << "bowel button got pressed" << std::endl;
		}, B3D_FULLSPLITRIGHT);

		m_GUIButton->addButton("../../resources/meshes/button3D/button3D.obj", "switch.png", "switch", glm::vec2(0, 0), [this]() {
			this->useOrgan = !this->useOrgan;
			if (useOrgan) {
				camera->SetAspectRatio(1.0f);
				camera->SetPosition(glm::vec3(0, -1.25f, 15.0f));
			}
			else {
				camera->SetAspectRatio(1.0f);
				camera->SetPosition(glm::vec3(0, 0.0f, 15.0f));
			}
		}, B3D_FULLSPLITLEFT);

		m_GUIButton->addButton("../../resources/meshes/button3D/button3D.obj", "split.jpg", "split", glm::vec2(-5, 4), [this]() {
			this->switchSplitscreen();
		}, B3D_FULLSPLITNONE);

		m_GUIButton->addButton("../../resources/meshes/button3D/button3D.obj", "merge.jpg", "merge", glm::vec2(-5, 4), [this]() {
			this->switchSplitscreen();
		}, B3D_SPLITLEFT);

		m_GUIButton->addButton("../../resources/meshes/button3D/button3D.obj", "SmartGesture.jpg", "smartgesture", glm::vec2(-5, 3), [this]() {
			this->switchQRState();
		}, B3D_FULLSPLITNONE);

		std::vector<std::string> matchList;
		matchList.push_back("lung_right");
		matchList.push_back("lung_left");
		m_organ->linkParentToChild("lungs", matchList);

		matchList.clear();
		matchList.push_back("heart");
		m_organ->linkParentToChild("heart", matchList);

		matchList.clear();
		matchList.push_back("stomach");
		m_organ->linkParentToChild("stomach", matchList);

		matchList.clear();
		matchList.push_back("liver");
		m_organ->linkParentToChild("liver", matchList);

		matchList.clear();
		matchList.push_back("bowel");
		m_organ->linkParentToChild("bowel", matchList);
			
		m_organButton->initScreenPosition();
		m_GUIButton->initScreenPosition();

		std::cout << "finished" << std::endl; //##################################################################
		glfwShowWindow(m_window);

		qrcode = m_cfg->CH_GenerateQR(getIPv4Address());

		cv::resize(qrcode, qrcode, cv::Size(static_cast<int>(m_windowDimensions.y * qrRelativeSize), static_cast<int>(m_windowDimensions.y * qrRelativeSize)), 0, 0, 0);

		marker = m_cfg->CH_GenerateMarker(m_windowDimensions);

		clickDelay = std::clock();

	}

	auto virtual InitGL() -> void override
	{


		initSmartGesture();


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

		glm::mat4 matPerspective = glm::transpose(glm::mat4(f_x, 0.0, -x_0, 0.0,
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

		camera = std::make_shared<Camera>();
		camera->SetAspectRatio(1.0f);
		camera->SetPosition(glm::vec3(0, -1.25f, 15.0f));

		backgroundQuad = std::make_shared<BackgroundQuad>();

		// Textures
		tex_background = std::make_shared<Texture>(GL_TEXTURE_2D, nullptr, m_windowDimensions.x, m_windowDimensions.y);
		tex_splitscreenBackground = std::make_shared<Texture>(GL_TEXTURE_2D, nullptr, m_windowDimensions.x / 2, m_windowDimensions.y);
		tex_marker = std::make_shared<Texture>(GL_TEXTURE_2D, marker.data, marker.cols, marker.rows);

		// Framebuffer Objects
		fbo_arView = std::make_shared<FramebufferObject>();
		fbo_arView->AttachTexture2D(tex_background->GetHandle());
		fbo_arView->AttachRenderbuffer(FramebufferObject::RenderBufferAttachement::DEPTH, GL_DEPTH_COMPONENT, m_windowDimensions.x, m_windowDimensions.y);
		fbo_arView->Validate();

		// Programs
		prg_imageQuad = std::make_shared<ShaderProgram>("../../resources/shaders/QR.vs", "../../resources/shaders/QR.fs");
		prg_lighting = std::make_shared<ShaderProgram>("../../resources/shaders/TEXTURE_ASSIMP.vs", "../../resources/shaders/TEXTURE_ASSIMP.fs");
		prg_singleColor = std::make_shared<ShaderProgram>("../../resources/shaders/shaderSingleColor.vs", "../../resources/shaders/shaderSingleColor.fs");

	}

	auto virtual Update() -> void override
	{

		//bool overTenSec = false;
		duration = (std::clock() - resetTimer) / (double)CLOCKS_PER_SEC;

		/*if (duration > 10) {
			for (std::list<Button>::iterator b = buttonContainer.begin(), end = buttonContainer.end(); b != end; ++b) {
#if !DEBUG
				b->setVisible(false);
#endif
			}
			overTenSec = true;
		}*/


		if (duration > 20) {
			doCalibration = false;
			qrDetected = false;
			resetTimer = clock();
		}
#if DEBUG
		POINT p;
		if (GetCursorPos(&p))
		{

			m_mousePosition.x = static_cast<int>(p.x * 1.0 / m_cfg->screenRes_Width * m_windowDimensions.x);
			m_mousePosition.y = static_cast<int>(p.y * 1.0 / m_cfg->screenRes_Height * m_windowDimensions.y);
		}
#endif

		if (splitscreenEnabled) {
			m_mousePosition.x += static_cast<int>(0.25 * m_windowDimensions.x);
		}

		if (mouseClicked) {
			if(useOrgan)
				m_organButton->clickSelected();
			m_GUIButton->clickSelected();
		}

		mouseClicked = false;

		if (m_kinectDataManger.GetDeviceOptions()->IsColorEnabled())
		{
			hr = m_kinectDataManger.Update();
			if (SUCCEEDED(hr))
			{
				m_colorFrame = m_kinectDataManger.GetColorFrame();

				if (!qrDetected && enableQR) {
					cv::Rect roi = cv::Rect(0, 0, qrcode.cols, qrcode.rows);
					cv::Mat subview = m_colorFrame(roi);
					qrcode.copyTo(subview);
				}

				// draw cursor
				//cv::circle(m_colorFrame, cv::Point(static_cast<int>(m_mousePosition.x), static_cast<int>(m_mousePosition.y)), 25, cv::Scalar(0, 0, 255, 255), -1, CV_AA);

				if (m_kinectDataManger.GetDeviceOptions()->IsBodyEnabled())
				{
					m_bodies = m_kinectDataManger.GetBodies();
					// Retrive tracking ID of closest user
					std::shared_ptr<Body> closestBody{ nullptr };

					if (!m_bodies.empty()) {
						for (const auto& currentBody : m_bodies)
						{
							if (closestBody == nullptr)
							{
								closestBody = currentBody;
							}
							else
							{
								if (glm::abs(currentBody->GetJointMap().at(JointType_SpineMid).GetWorldPosition().Z) < glm::abs(closestBody->GetJointMap().at(JointType_SpineMid).GetWorldPosition().Z))
								{
									closestBody = currentBody;
								}
							}
							if (closestBody != nullptr)
							{
								m_closestBody = closestBody;
							}
						}
					}
					else {
						m_closestBody = nullptr;
					}

					if (!qrDetected && m_closestBody != nullptr) {
						//mouse controlling by kinect user
						//##############################################################################################################

						//Retrieve the starting point and passing point (fingertip) of the pointing vector
						CameraSpacePoint pHead = m_closestBody->GetJointMap().find(JointType_SpineMid)->second.GetWorldPosition();
						glm::vec3 eyePosition(pHead.X, pHead.Y, pHead.Z);

						CameraSpacePoint pHand = m_closestBody->GetJointMap().find(JointType_HandRight)->second.GetWorldPosition();
						glm::vec3 fingertip(pHand.X, pHand.Y, pHand.Z);

						//compute the screen coordinate from the pointing ray
						glm::vec3 pScreen = m_cursorUser->smoothedRayPointing(eyePosition, fingertip);

						//add screen coordinate and retrieve smoothed screen coordinate
						m_cursorUser->addEntry(pScreen);
						glm::vec3 cursorPos = m_cursorUser->getSmoothedPosition();

						//Retrieve world coordinate of the user's right hand and the pointing direction
						CameraSpacePoint csp_Index = m_closestBody->GetJointMap().find(JointType_HandTipRight)->second.GetWorldPosition();
						CameraSpacePoint csp_Wrist = m_closestBody->GetJointMap().find(JointType_WristRight)->second.GetWorldPosition();
						glm::vec3 p_IndexFingertip = glm::vec3(csp_Index.X - csp_Wrist.X, csp_Index.Y - csp_Wrist.Y, csp_Index.Z - csp_Wrist.Z) * 1000.0f;
						

						if (cursorPos.x >= 0 && cursorPos.x <= m_cfg->screenRes_Width && cursorPos.y >= 0 && cursorPos.y <= m_cfg->screenRes_Height) {
							//check if the right hand performs a click by producing an outlier downwards
							mouseClicked = m_fingertip->clickVertical(p_IndexFingertip);
							
							SetCursorPos(cursorPos.x, cursorPos.y);		//set cursor to the smoothed screen coordinate

							m_mousePosition.x = cursorPos.x;
							m_mousePosition.y = cursorPos.y;
						}

						//handle click history.
						if (mouseClicked) {
							m_fingertip->setAll(p_IndexFingertip);
						}
						else {
							m_fingertip->addEntry(p_IndexFingertip);
						}
						//############################################################################################################
					}
				}

				tex_background->UpdateTexture(m_colorFrame.data, 0, m_colorFrame.cols, 0, m_colorFrame.rows);
				if (splitscreenEnabled)
				{
					cv::Mat colorFrameSplitScreen = m_colorFrame(cv::Range::all(), cv::Range(m_colorFrame.cols / 4, m_colorFrame.cols - m_colorFrame.cols / 4));
					cv::Mat B = colorFrameSplitScreen.clone();
					tex_splitscreenBackground->UpdateTexture(B.data, 0, colorFrameSplitScreen.cols, 0, colorFrameSplitScreen.rows);
				}
			}
		}

	}

	auto virtual Draw() -> void override
	{

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		int windowWidth, windowHeight;
		glfwGetWindowSize(m_window, &windowWidth, &windowHeight);

		if (!splitscreenEnabled || doCalibration)
		{

			glViewport(0, 0, windowWidth, windowHeight);

			// Render Kinect data stream as texture
			glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			glDisable(GL_DEPTH_TEST);

			prg_imageQuad->UseProgram();
			prg_imageQuad->SetUniform("modelMatrix", glm::mat4());
			prg_imageQuad->SetUniform("doCalibration", doCalibration);
			glActiveTexture(GL_TEXTURE0);
			tex_background->Bind();
			glActiveTexture(GL_TEXTURE1);
			tex_marker->Bind();
			backgroundQuad->Render();
			tex_marker->Unbind();
			glActiveTexture(GL_TEXTURE0);
			tex_background->Unbind();
			prg_imageQuad->UnUseProgram();


			if (m_kinectDataManger.GetDeviceOptions()->IsBodyEnabled() && !doCalibration)
			{

				// Loop over all bodies and draw spheres at skeletal joint positions
				//for (const auto& currentBody : m_bodies)
				if(m_closestBody != nullptr)
				{
					double organScale;

					Vector4 orientation(m_closestBody->GetJointMap().find(JointType_SpineMid)->second.GetOrientation());
					glm::quat quat(orientation.w, orientation.x, orientation.y, orientation.z);

					glm::vec3 pyr = glm::eulerAngles(quat);

					//Calculate scale for organs with 1m SpineShoulder to SpineBase as reference
					CameraSpacePoint wPos_SpineBase(m_closestBody->GetJointMap().find(JointType_SpineBase)->second.GetWorldPosition());
					CameraSpacePoint wPos_ShoulderLeft(m_closestBody->GetJointMap().find(JointType_ShoulderLeft)->second.GetWorldPosition());
					CameraSpacePoint wPos_ShoulderRight(m_closestBody->GetJointMap().find(JointType_ShoulderRight)->second.GetWorldPosition());

					glm::vec3 spineLength(wPos_SpineBase.X - (wPos_ShoulderLeft.X + wPos_ShoulderRight.X) / 2,
						wPos_SpineBase.Y - (wPos_ShoulderLeft.Y + wPos_ShoulderRight.Y) / 2,
						wPos_SpineBase.Z - (wPos_ShoulderLeft.Z + wPos_ShoulderRight.Z) / 2);
					organScale = glm::sqrt(spineLength.x * spineLength.x + spineLength.y * spineLength.y + spineLength.z * spineLength.z);


					glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
					glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(V));
					glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(P));
					glBindBuffer(GL_UNIFORM_BUFFER, 0);

					// Draw organs
					CameraSpacePoint p = m_closestBody->GetJointMap().find(JointType_SpineMid)->second.GetWorldPosition();
					glm::vec3 worldCoordinate = glm::vec3(p.X, p.Y, p.Z);
					if (useOrgan) {
						m_organ->draw(prg_lighting, prg_singleColor, worldCoordinate, pyr, glm::vec3(organScale, organScale, organScale), true, isMirror);
					}
					else {
						m_digSys->draw(prg_lighting, prg_singleColor, worldCoordinate, pyr, 2.0f * glm::vec3(organScale, organScale, organScale), true, isMirror);
					}
				}
			}
			//Drawing GUI buttons
#if !DEBUG
			if (qrDetected && !doCalibration) {
#endif
				if (!doCalibration) {
					if (useOrgan) {
						m_organButton->draw(prg_lighting, prg_singleColor, &ubo_matrices);
					}
					m_GUIButton->draw(prg_lighting, prg_singleColor, &ubo_matrices);
				}
#if !DEBUG
			}
#endif
		}
		else
		{
			//Draw left screen  -----------------------------------------------------------------------------------------------------------------
			glViewport(0, 0, windowWidth / 2, windowHeight);

			glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			glDisable(GL_DEPTH_TEST);

			prg_imageQuad->UseProgram();
			prg_imageQuad->SetUniform("modelMatrix", glm::mat4());
			glActiveTexture(GL_TEXTURE0);
			tex_splitscreenBackground->Bind();
			backgroundQuad->Render();
			tex_splitscreenBackground->Unbind();
			prg_imageQuad->UnUseProgram();

			glm::vec3 pyr;

			if (m_closestBody != nullptr)
			{
				double organScale;

				Vector4 orientation(m_closestBody->GetJointMap().find(JointType_SpineMid)->second.GetOrientation());
				glm::quat quat(orientation.w, orientation.x, orientation.y, orientation.z);

				pyr = glm::eulerAngles(quat);

				//Calculate scale for organs with 1m SpineShoulder to SpineBase as reference
				CameraSpacePoint wPos_SpineBase(m_closestBody->GetJointMap().find(JointType_SpineBase)->second.GetWorldPosition());
				CameraSpacePoint wPos_ShoulderLeft(m_closestBody->GetJointMap().find(JointType_ShoulderLeft)->second.GetWorldPosition());
				CameraSpacePoint wPos_ShoulderRight(m_closestBody->GetJointMap().find(JointType_ShoulderRight)->second.GetWorldPosition());

				glm::vec3 spineLength(wPos_SpineBase.X - (wPos_ShoulderLeft.X + wPos_ShoulderRight.X) / 2,
					wPos_SpineBase.Y - (wPos_ShoulderLeft.Y + wPos_ShoulderRight.Y) / 2,
					wPos_SpineBase.Z - (wPos_ShoulderLeft.Z + wPos_ShoulderRight.Z) / 2);
				organScale = glm::sqrt(spineLength.x * spineLength.x + spineLength.y * spineLength.y + spineLength.z * spineLength.z);


				glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(V));
				glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(P_splitscreen));
				glBindBuffer(GL_UNIFORM_BUFFER, 0);

				// Draw organs
				CameraSpacePoint p = m_closestBody->GetJointMap().find(JointType_SpineMid)->second.GetWorldPosition();
				glm::vec3 worldCoordinate = glm::vec3(p.X, p.Y, p.Z);
				if (useOrgan) {
					m_organ->draw(prg_lighting, prg_singleColor, worldCoordinate, pyr, glm::vec3(organScale, organScale, organScale), true, isMirror);
				}
				else {
					m_digSys->draw(prg_lighting, prg_singleColor, worldCoordinate, pyr, 2.0f * glm::vec3(organScale, organScale, organScale), true, isMirror);
				}

				
				
			}
			m_GUIButton->draw(prg_lighting, prg_singleColor, &ubo_matrices, B3D_FROMLEFT);
			//END Draw left screen ---------------------------------------------------------------------------------------------------------------
			//Draw right screen  -----------------------------------------------------------------------------------------------------------------
			glViewport(windowWidth / 2, 0, windowWidth / 2, windowHeight);

			glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->GetViewMatrix()));
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera->GetProjectionMatrix()));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			if (m_closestBody != nullptr)
			{
				if (useOrgan) {
					m_organ->draw(prg_lighting, prg_singleColor, glm::vec3(0, pyr[1], 0), glm::vec3(1.2f, 1.2f, 1.2f), true);
				}
				else {
					m_digSys->draw(prg_lighting, prg_singleColor, glm::vec3(0, pyr[1], 0), glm::vec3(1.5f, 1.5f, 1.5f), true);
				}
				
				//END Draw right screen --------------------------------------------------------------------------------------------------------------
			}
			//Drawing GUI buttons
#if !DEBUG
			if (qrDetected) {
#endif
				if (useOrgan && !doCalibration) {
					m_organButton->draw(prg_lighting, prg_singleColor, &ubo_matrices, B3D_FROMRIGHT);
				}
				m_GUIButton->draw(prg_lighting, prg_singleColor, &ubo_matrices, B3D_FROMRIGHT);
#if !DEBUG
			}
#endif

		}
#if SHOWFPS
		double lastFrame = (std::clock() - fps) / (double)CLOCKS_PER_SEC;
		std::cout << "\r                  ";
		std::cout << "\r FPS: " << (int)(1 / lastFrame);
		fps = clock();
#endif
	}

	auto virtual ShutdownGL() -> void override
	{
		GLUtils::BufferUtils::DeleteBufferObject(&ubo_matrices);
		GLUtils::BufferUtils::DeleteBufferObject(&ubo_lightInfo);
		GLUtils::BufferUtils::DeleteBufferObject(&ubo_materialInfo);
	}

	auto virtual Cleanup() -> void override
	{
		shutdown_Server();

		hr = m_kinectDataManger.CloseKinectSensor();
		if (FAILED(hr))
		{
			std::string error{ "Error while trying to close Kinect sensor!" };
			std::cerr << error << std::endl;
			throw std::runtime_error(error.c_str());
		}
	}

	// Switches between full screen and split screen
	void switchSplitscreen() {
		splitscreenEnabled = !splitscreenEnabled;
		m_organButton->switchSplitscreen(splitscreenEnabled);
		m_GUIButton->switchSplitscreen(splitscreenEnabled);
	}

	void switchQRState() {
		enableQR = !enableQR;
	}

private:

	bool useOrgan{ true };
	std::shared_ptr<Camera> camera{ nullptr };
	std::clock_t resetTimer;
	std::clock_t clickDelay;

	double duration{ 0 };

#if SHOWFPS
	std::clock_t fps;
#endif


	std::shared_ptr<ConfigHandler> m_cfg;
	std::shared_ptr<CursorHistory> m_cursorUser;
	std::shared_ptr<CursorHistory> m_fingertip;


	std::string m_windowTitle{ "GLFWKinectTest" };
	glm::uvec2 m_windowDimensions{ glm::uvec2(800, 600) };
	glm::ivec2 m_windowPosition{ glm::ivec2(0, 0) };

	HRESULT hr{ S_OK };
	KinectDataManager& m_kinectDataManger{ KinectDataManager::GetInstance() };
	cv::Mat m_colorFrame;
	std::vector<std::shared_ptr<Body>> m_bodies{};

	
	std::shared_ptr<OrganModel> m_organ;
	std::shared_ptr<OrganModel> m_digSys;

	bool isMirror{ true };

	std::shared_ptr<ButtonsVR> m_organButton;
	std::shared_ptr<ButtonsVR> m_GUIButton;

	std::shared_ptr<Body> m_closestBody{ nullptr };

	GLuint ubo_matrices{ 0 };
	GLuint ubo_lightInfo{ 0 };
	GLuint ubo_materialInfo{ 0 };

	std::shared_ptr<BackgroundQuad> backgroundQuad{ nullptr };
	

	std::shared_ptr<Texture> tex_background{ nullptr };
	std::shared_ptr<Texture> tex_marker{ nullptr };
	std::shared_ptr<Texture> tex_splitscreenBackground{ nullptr };

	std::shared_ptr<FramebufferObject> fbo_arView{ nullptr };

	std::shared_ptr<ShaderProgram> prg_imageQuad{ nullptr };
	std::shared_ptr<ShaderProgram> prg_lighting{ nullptr };
	std::shared_ptr<ShaderProgram> prg_singleColor{ nullptr };

	cv::Mat qrcode;
	bool qrDetected{ false };
	bool enableQR{ false };
	double qrRelativeSize{ 0.3 };

	cv::Mat marker;
	bool doCalibration{ false };

	glm::ivec2 m_mousePosition{ glm::ivec2(0, 0) };

	bool splitscreenEnabled{ false };
	bool mouseClicked = false;


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
};



int main(int argc, char ** argv)
{

	std::thread m_iosThread;
	try {
		asio::io_service io_service;
		SmartGestureApp m_sma(io_service);
		m_iosThread = std::thread(std::bind(static_cast<size_t(asio::io_service::*)()>(&asio::io_service::run), &io_service));

		m_sma.Run();
		m_iosThread.detach();

		exit(1);
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