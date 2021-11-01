﻿#include "CommonKinect.h"
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

class SkeletonJointsSphereOverlay : public GLFWApp
{
public:
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
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

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

        glm::mat4 matPerspective = glm::transpose(glm::mat4(f_x,  0.0,  -x_0,    0.0,
                                                            0.0,  f_y,  -y_0,    0.0,
                                                            0.0,  0.0,  N + F,  N * F,
                                                            0.0,  0.0,  -1.0,    0.0));

        glm::mat4 matNDC = glm::transpose(glm::mat4(2.0 / (R - L),      0.0,           0.0,       -(R + L) / (R - L),  // Equivalent to glm::ortho(left, right, bottom, top, near, far)
                                                         0.0,      2.0 / (T - B),      0.0,       -(T + B) / (T - B),
                                                         0.0,           0.0,      -2.0 / (F - N), -(F + N) / (F - N),
                                                         0.0,           0.0,           0.0,               1.0));

        P = matNDC * matPerspective;

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

		// Pointers
		sphere = std::make_shared<Sphere>(10.0f, 20, 20);
		backgroundQuad = std::make_shared<BackgroundQuad>();

		// Textures
		tex_background = std::make_shared<Texture>(GL_TEXTURE_2D, nullptr, m_windowDimensions.x, m_windowDimensions.y);

		// Programs
		prg_imageQuad = std::make_shared<ShaderProgram>("../../resources/shaders/TEXTURE.vs", "../../resources/shaders/TEXTURE.fs");
		prg_sphere = std::make_shared<ShaderProgram>("../../resources/shaders/SIMPLE.vs", "../../resources/shaders/SIMPLE.fs");
    }

	auto virtual Update() -> void override
	{
		if (m_kinectDataManger.GetDeviceOptions()->IsColorEnabled())
		{
			HRESULT hr = m_kinectDataManger.Update();
			if (SUCCEEDED(hr))
			{
				cv::Mat colorFrame = m_kinectDataManger.GetColorFrame();

				if (m_kinectDataManger.GetDeviceOptions()->IsBodyEnabled())
				{
					m_bodies = m_kinectDataManger.GetBodies();
				}

				tex_background->UpdateTexture(colorFrame.data, 0, colorFrame.cols, 0, colorFrame.rows);
			}
		}
	}

    auto virtual Draw() -> void override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render Kinect data stream as texture
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

		// Loop over all bodies and draw spheres at skeletal joint positions
		for (const auto& currentBody : m_bodies)
		{
			for (const auto& currentJoint : currentBody->GetJointMap())
			{
				CameraSpacePoint currentjointPositionWorldSpace = currentJoint.second.GetWorldPosition();
				TrackingState currentJointTrackingState = currentJoint.second.GetTrackingState();

				glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
					glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(V));
					glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(P));
				glBindBuffer(GL_UNIFORM_BUFFER, 0);

				prg_sphere->UseProgram();
					sphere->ResetModelMatrix();
					sphere->ScaleModel(glm::vec3(static_cast<int>(currentJointTrackingState), static_cast<int>(currentJointTrackingState), static_cast<int>(currentJointTrackingState)));
					sphere->TranslateModel(1000.0f * glm::vec3(currentjointPositionWorldSpace.X, currentjointPositionWorldSpace.Y, currentjointPositionWorldSpace.Z));
					prg_sphere->SetUniform("modelMatrix", sphere->GetModelMatrix());
					sphere->Render(GL_POINTS);
				prg_sphere->UnUseProgram();
			}
		}
    }

    auto virtual ShutdownGL() -> void override
    {
        GLUtils::BufferUtils::DeleteBufferObject(&ubo_matrices);
    }

    auto virtual Cleanup() -> void override
    {
        HRESULT hr = m_kinectDataManger.CloseKinectSensor();
        if (FAILED(hr))
        {
            std::string error{"Error while trying to close Kinect sensor!"};
            std::cerr << error << std::endl;
            throw std::runtime_error(error.c_str());
        }
    }

private:
    KinectDataManager& m_kinectDataManger{KinectDataManager::GetInstance()};

	std::vector<std::shared_ptr<Body>> m_bodies{};

    GLuint ubo_matrices{0};

	std::shared_ptr<Sphere> sphere{ nullptr };
	std::shared_ptr<BackgroundQuad> backgroundQuad{ nullptr };

	std::shared_ptr<Texture> tex_background{ nullptr };

	std::shared_ptr<ShaderProgram> prg_imageQuad{ nullptr };
	std::shared_ptr<ShaderProgram> prg_sphere{ nullptr };

	glm::mat4 P{ glm::mat4() };
	glm::mat4 V{ glm::mat4() };

    // Kinect 1 Calibration Parameters
    double f_x{1.0555595295248311e+003};
    double f_y{1.0553695516063312e+003};
    double x_0{9.6845279782509590e+002};
    double y_0{5.2657689367910041e+002};
    glm::mat4 E{glm::transpose(glm::mat4( 9.9996929470031348e-001, 6.0588270175632817e-003,  4.9699367932315398e-003,  5.1917725950575111e+001,
                                         -6.0572078150210925e-003, 9.9998159687953281e-001, -3.4078694968998495e-004, -3.7189925174652749e-001,
                                         -4.9719101000640373e-003, 3.1067254574039988e-004,  9.9998759171928042e-001, -2.1870288086129372e+000, 
                                                   0.0,                     0.0,                     0.0,                       1.0))};
    std::vector<double> distortionParameters{{4.2452333236524954e-002, -4.2231307253447571e-002, 9.6967086533845590e-004, 3.7297236935803001e-004, -1.1455528267188553e-003}};
    double projectionError{2.5731037797761691e-001};
    

    // Kinect 2 Calibration Parameters
    /*
    double f_x{1.0590207378474561e+003};
    double f_y{1.0602278248866719e+003};
    double x_0{9.6906880467301505e+002};
    double y_0{5.5763522351764993e+002};
    glm::mat4 E{glm::transpose(glm::mat4( 9.9997707218768395e-001, 5.9835795424456346e-003,  3.1704691776957821e-003, 5.1985183305662048e+001,
                                         -5.9738187566786803e-003, 9.9997741564810050e-001, -3.0792341918627732e-003, 3.1884728927408262e-001,
                                         -3.1888224174210240e-003, 3.0602237835179458e-003, 9.9999023317329705e-001, -5.5261241122233029e-001, 
                                                   0.0,                     0.0,                     0.0,                       1.0))};
    std::vector<double> distortionParameters{{4.8155798581069449e-002, -4.3824054985386704e-002, 9.4719372128870970e-005, 8.2384392025622820e-004, -1.1854187597704079e-002}};
    double projectionError{2.3462617284774229e-001};
    */
};

int main(int argc, char ** argv)
{
    try
    {
        return SkeletonJointsSphereOverlay().Run();
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