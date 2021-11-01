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

class SkeletonLinesOpenCV : public GLFWApp
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

		// UBO Matrices (binding index 0)
		ubo_matrices = GLUtils::BufferUtils::CreateBufferObject(GL_UNIFORM_BUFFER, nullptr, 2 * sizeof(glm::mat4), GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_matrices);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Pointers
		backgroundQuad = std::make_shared<BackgroundQuad>();

		// Textures
		tex_background = std::make_shared<Texture>(GL_TEXTURE_2D, nullptr, m_windowDimensions.x, m_windowDimensions.y);

		// Programs
		prg_imageQuad = std::make_shared<ShaderProgram>("../../resources/shaders/TEXTURE.vs", "../../resources/shaders/TEXTURE.fs");
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
					std::vector<std::shared_ptr<Body>> bodies = m_kinectDataManger.GetBodies();

					// Loop over all bodies and draw skeletal joints as points and bones as lines
					// using OpenCV and update the colorFrame with the points and lines.
					for (const auto& currentBody : bodies)
					{
						// Color hands depending on state
						HandState leftHandState = currentBody->GetLeftHandState();
						HandState rightHandState = currentBody->GetRightHandState();

						ColorSpacePoint screenPositionLeftHand  = ColorSpacePoint{ 0, 0 };
						ColorSpacePoint screenPositionRightHand = ColorSpacePoint{ 0, 0 };
						HRESULT hr1 = m_kinectDataManger.GetCoordinateMapper()->MapCameraPointToColorSpace(currentBody->GetJointMap().at(JointType_HandLeft).GetWorldPosition(), &screenPositionLeftHand);
						HRESULT hr2 = m_kinectDataManger.GetCoordinateMapper()->MapCameraPointToColorSpace(currentBody->GetJointMap().at(JointType_HandLeft).GetWorldPosition(), &screenPositionRightHand);

						if (SUCCEEDED(hr1) && SUCCEEDED(hr2))
						{
							DrawHand(screenPositionLeftHand, leftHandState, colorFrame);
							DrawHand(screenPositionRightHand, rightHandState, colorFrame);
						}

						// Draw joints and bones
						DrawBody(currentBody, colorFrame);
					}
				}

				tex_background->UpdateTexture(colorFrame.data, 0, colorFrame.cols, 0, colorFrame.rows);
			}
		}
	}

    auto virtual Draw() -> void override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        prg_imageQuad->UseProgram();
			prg_imageQuad->SetUniform("modelMatrix", glm::mat4());
			glActiveTexture(GL_TEXTURE0);
				tex_background->Bind();
					backgroundQuad->Render();
				tex_background->Unbind();
        prg_imageQuad->UnUseProgram();
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

    auto DrawBody(const std::shared_ptr<Body>& body, const cv::Mat& imageMat) -> void
    {
        auto jointMap = body->GetJointMap();

        // Color joints
        for (const auto& joint : jointMap)
        {
			ColorSpacePoint screenPositionJoint = ColorSpacePoint{ 0, 0 };
			HRESULT hr = m_kinectDataManger.GetCoordinateMapper()->MapCameraPointToColorSpace(joint.second.GetWorldPosition(), &screenPositionJoint);

			if (SUCCEEDED(hr))
			{
				int x = static_cast<int>(screenPositionJoint.X);
				int y = static_cast<int>(screenPositionJoint.Y);

				if ((x >= 0) && (x < imageMat.cols) && (y >= 0) && (y < imageMat.rows))
				{
					cv::circle(imageMat, cv::Point(x, y), 5, cv::Scalar(0, 0, 255), -1, CV_AA);
				}
			}
        }

        Body::BodyJoint spineBaseJoint = jointMap.at(JointType_SpineBase);
        Body::BodyJoint spineMidJoint = jointMap.at(JointType_SpineMid);
        Body::BodyJoint neckJoint = jointMap.at(JointType_Neck);
        Body::BodyJoint headJoint = jointMap.at(JointType_Head);
        Body::BodyJoint shoulderLeftJoint = jointMap.at(JointType_ShoulderLeft);
        Body::BodyJoint elbowLeftJoint = jointMap.at(JointType_ElbowLeft);
        Body::BodyJoint wristLeftJoint = jointMap.at(JointType_WristLeft);
        Body::BodyJoint handLeftJoint = jointMap.at(JointType_HandLeft);
        Body::BodyJoint shoulderRightJoint = jointMap.at(JointType_ShoulderRight);
        Body::BodyJoint elbowRightJoint = jointMap.at(JointType_ElbowRight);
        Body::BodyJoint wristRightJoint = jointMap.at(JointType_WristRight);
        Body::BodyJoint handRightJoint = jointMap.at(JointType_HandRight);
        Body::BodyJoint hipLeftJoint = jointMap.at(JointType_HipLeft);
        Body::BodyJoint kneeLeftJoint = jointMap.at(JointType_KneeLeft);
        Body::BodyJoint ankleLeftJoint = jointMap.at(JointType_AnkleLeft);
        Body::BodyJoint footLeftJoint = jointMap.at(JointType_FootLeft);
        Body::BodyJoint hipRightJoint = jointMap.at(JointType_HipRight);
        Body::BodyJoint kneeRightJoint = jointMap.at(JointType_KneeRight);
        Body::BodyJoint ankleRightJoint = jointMap.at(JointType_AnkleRight);
        Body::BodyJoint footRightJoint = jointMap.at(JointType_FootRight);
        Body::BodyJoint spineShoulderJoint = jointMap.at(JointType_SpineShoulder);
        Body::BodyJoint handTipLeftJoint = jointMap.at(JointType_HandTipLeft);
        Body::BodyJoint thumbLeftJoint = jointMap.at(JointType_ThumbLeft);
        Body::BodyJoint handTipRightJoint = jointMap.at(JointType_HandTipRight);
        Body::BodyJoint thumbRightJoint = jointMap.at(JointType_ThumbRight);

        cv::Scalar colorTorso = cv::Scalar(0, 255, 0);
        cv::Scalar colorArmLeft = cv::Scalar(255, 153, 204);
        cv::Scalar colorArmRight = cv::Scalar(153, 153, 255);
        cv::Scalar colorLegLeft = cv::Scalar(255, 255, 0);
        cv::Scalar colorLegRight = cv::Scalar(255, 153, 0);

        // Torso
        DrawBone(headJoint, neckJoint, imageMat, colorTorso);
        DrawBone(neckJoint, spineShoulderJoint, imageMat, colorTorso);
        DrawBone(spineShoulderJoint, spineMidJoint, imageMat, colorTorso);
        DrawBone(spineMidJoint, spineBaseJoint, imageMat, colorTorso);
        DrawBone(spineShoulderJoint, shoulderLeftJoint, imageMat, colorTorso);
        DrawBone(spineShoulderJoint, shoulderRightJoint, imageMat, colorTorso);
        DrawBone(spineBaseJoint, hipLeftJoint, imageMat, colorTorso);
        DrawBone(spineBaseJoint, hipRightJoint, imageMat, colorTorso);

        // Left Arm
        DrawBone(shoulderLeftJoint, elbowLeftJoint, imageMat, colorArmLeft);
        DrawBone(elbowLeftJoint, wristLeftJoint, imageMat, colorArmLeft);
        DrawBone(wristLeftJoint, handLeftJoint, imageMat, colorArmLeft);
        DrawBone(handLeftJoint, handTipLeftJoint, imageMat, colorArmLeft);
        DrawBone(wristLeftJoint, thumbLeftJoint, imageMat, colorArmLeft);

        // Right Arm
        DrawBone(shoulderRightJoint, elbowRightJoint, imageMat, colorArmRight);
        DrawBone(elbowRightJoint, wristRightJoint, imageMat, colorArmRight);
        DrawBone(wristRightJoint, handRightJoint, imageMat, colorArmRight);
        DrawBone(handRightJoint, handTipRightJoint, imageMat, colorArmRight);
        DrawBone(wristRightJoint, thumbRightJoint, imageMat, colorArmRight);

        // Left Leg
        DrawBone(hipLeftJoint, kneeLeftJoint, imageMat, colorLegLeft);
        DrawBone(kneeLeftJoint, ankleLeftJoint, imageMat, colorLegLeft);
        DrawBone(ankleLeftJoint, footLeftJoint, imageMat, colorLegLeft);

        // Right Leg
        DrawBone(hipRightJoint, kneeRightJoint, imageMat, colorLegRight);
        DrawBone(kneeRightJoint, ankleRightJoint, imageMat, colorLegRight);
        DrawBone(ankleRightJoint, footRightJoint, imageMat, colorLegRight);
    }

    auto DrawBone(const Body::BodyJoint& joint1, const Body::BodyJoint& joint2, const cv::Mat& imageMat, const cv::Scalar& boneColor) -> void
    {
        const int LineWidthInferred = 1;
        const int LineWidthTracked = 6;

        TrackingState trackingStateJoint1 = joint1.GetTrackingState();
        TrackingState trackingStateJoint2 = joint2.GetTrackingState();

        if ((trackingStateJoint1 == TrackingState_NotTracked) || (trackingStateJoint2 == TrackingState_NotTracked))
        {
            return;
        }

        if ((trackingStateJoint1 == TrackingState_Inferred) && (trackingStateJoint2 == TrackingState_Inferred))
        {
            return;
        }

		ColorSpacePoint screenPositionJoint1 = ColorSpacePoint{0, 0};
		ColorSpacePoint screenPositionJoint2 = ColorSpacePoint{0, 0};
		HRESULT hr1 = m_kinectDataManger.GetCoordinateMapper()->MapCameraPointToColorSpace(joint1.GetWorldPosition(), &screenPositionJoint1);
		HRESULT hr2 = m_kinectDataManger.GetCoordinateMapper()->MapCameraPointToColorSpace(joint2.GetWorldPosition(), &screenPositionJoint2);

		if (SUCCEEDED(hr1) && SUCCEEDED(hr2))
		{
			if ((trackingStateJoint1 == TrackingState_Tracked) && (trackingStateJoint2 == TrackingState_Tracked))
			{
				cv::line(imageMat, cv::Point(static_cast<int>(screenPositionJoint1.X), static_cast<int>(screenPositionJoint1.Y)), cv::Point(static_cast<int>(screenPositionJoint2.X), static_cast<int>(screenPositionJoint2.Y)), boneColor, LineWidthTracked, CV_AA);
			}
			else
			{
				cv::line(imageMat, cv::Point(static_cast<int>(screenPositionJoint1.X), static_cast<int>(screenPositionJoint1.Y)), cv::Point(static_cast<int>(screenPositionJoint2.X), static_cast<int>(screenPositionJoint2.Y)), boneColor, LineWidthInferred, CV_AA);
			}
		}
    }

    auto DrawHand(const ColorSpacePoint& position, const HandState& handState, const cv::Mat& imageMat) -> void
    {
        int x = static_cast<int>(position.X);
        int y = static_cast<int>(position.Y);

        if ((x >= 0) && (x < imageMat.cols) && (y >= 0) && (y < imageMat.rows))
        {
            switch (handState)
            {
                case HandState_Open:
                    cv::circle(imageMat, cv::Point(x, y), 75, cv::Scalar(0, 128, 0), 5, CV_AA);
                    break;

                case HandState_Closed:
                    cv::circle(imageMat, cv::Point(x, y), 75, cv::Scalar(0, 0, 128), 5, CV_AA);
                    break;

                case HandState_Lasso:
                    cv::circle(imageMat, cv::Point(x, y), 75, cv::Scalar(128, 128, 0), 5, CV_AA);
                    break;

                case HandState_Unknown:
                    cv::circle(imageMat, cv::Point(x, y), 75, cv::Scalar(128, 128, 128), 5, CV_AA);
                    break;

                case HandState_NotTracked:
                    break;

                default: break;
            }
        }
    }

private:
    KinectDataManager& m_kinectDataManger{KinectDataManager::GetInstance()};

    GLuint ubo_matrices{0};

	std::shared_ptr<BackgroundQuad> backgroundQuad{ nullptr };

	std::shared_ptr<Texture> tex_background{ nullptr };

	std::shared_ptr<ShaderProgram> prg_imageQuad{ nullptr };
};

int main(int argc, char ** argv)
{
    try
    {
        return SkeletonLinesOpenCV().Run();
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