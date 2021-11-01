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

class HDFaceTracking : public GLFWApp
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

					for (const auto& currentBody : bodies)
					{
						if (m_kinectDataManger.GetDeviceOptions()->IsHDFaceEnabled())
						{
							std::shared_ptr<Body::BodyHDFace> hdFace = currentBody->GetBodyHDFace();

							if (hdFace != nullptr)
							{
								// Draw face bounding rectangle
								RectI boundingRect = hdFace->GetBoundingBoxInColorSpace();
								cv::rectangle(colorFrame, cv::Rect(boundingRect.Left, boundingRect.Top, boundingRect.Right - boundingRect.Left, boundingRect.Bottom - boundingRect.Top), cv::Scalar(0, 0, 255));

								// Draw head pivot point
								ColorSpacePoint screenPositionHeadPivot = ColorSpacePoint{ 0, 0 };
								hr = m_kinectDataManger.GetCoordinateMapper()->MapCameraPointToColorSpace(hdFace->GetHeadPivotWorldPosition(), &screenPositionHeadPivot);

								if (SUCCEEDED(hr))
								{
									cv::circle(colorFrame, cv::Point(static_cast<int>(screenPositionHeadPivot.X), static_cast<int>(screenPositionHeadPivot.Y)), 5, cv::Scalar(0, 0, 255), -1, CV_AA);
								}

								// Draw HD face vertices
								std::vector<CameraSpacePoint> verticesWorldSpace = hdFace->GetVerticesWorldSpace();
								std::vector<ColorSpacePoint> verticesScreenSpace(verticesWorldSpace.size());
								hr = m_kinectDataManger.GetCoordinateMapper()->MapCameraPointsToColorSpace(static_cast<UINT>(verticesWorldSpace.size()), &verticesWorldSpace[0], static_cast<UINT>(verticesWorldSpace.size()), &verticesScreenSpace[0]);
								if (SUCCEEDED(hr))
								{
									for (const auto& currentVertex : verticesScreenSpace)
									{
										cv::circle(colorFrame, cv::Point(static_cast<int>(currentVertex.X), static_cast<int>(currentVertex.Y)), 1, cv::Scalar(255, 0, 0), -1, CV_AA);
									}
								}

								// Highlight extreme HD face vertices
								ColorSpacePoint minX = ColorSpacePoint{ 0, 0 };
								ColorSpacePoint maxX = ColorSpacePoint{ 0, 0 };
								ColorSpacePoint minY = ColorSpacePoint{ 0, 0 };
								ColorSpacePoint maxY = ColorSpacePoint{ 0, 0 };

								hr = m_kinectDataManger.GetCoordinateMapper()->MapCameraPointToColorSpace(hdFace->GetPointsMinMaxXWorldSpace().first, &minX);
								if (SUCCEEDED(hr))
								{
									cv::circle(colorFrame, cv::Point(static_cast<int>(minX.X), static_cast<int>(minX.Y)), 3, cv::Scalar(0, 255, 0), -1, CV_AA);
								}

								hr = m_kinectDataManger.GetCoordinateMapper()->MapCameraPointToColorSpace(hdFace->GetPointsMinMaxXWorldSpace().second, &maxX);
								if (SUCCEEDED(hr))
								{
									cv::circle(colorFrame, cv::Point(static_cast<int>(maxX.X), static_cast<int>(maxX.Y)), 3, cv::Scalar(0, 255, 0), -1, CV_AA);
								}

								hr = m_kinectDataManger.GetCoordinateMapper()->MapCameraPointToColorSpace(hdFace->GetPointsMinMaxYWorldSpace().first, &minY);
								if (SUCCEEDED(hr))
								{
									cv::circle(colorFrame, cv::Point(static_cast<int>(minY.X), static_cast<int>(minY.Y)), 3, cv::Scalar(0, 255, 0), -1, CV_AA);
								}

								hr = m_kinectDataManger.GetCoordinateMapper()->MapCameraPointToColorSpace(hdFace->GetPointsMinMaxYWorldSpace().second, &maxY);
								if (SUCCEEDED(hr))
								{
									cv::circle(colorFrame, cv::Point(static_cast<int>(maxY.X), static_cast<int>(maxY.Y)), 3, cv::Scalar(0, 255, 0), -1, CV_AA);
								}

								// Draw ellipse around convex hull of HD face vertices
								std::vector<cv::Point> convexHull;

								for (const auto& currentVertex : verticesScreenSpace)
								{
									convexHull.push_back(cv::Point(static_cast<int>(currentVertex.X), static_cast<int>(currentVertex.Y)));
								}

								cv::convexHull(convexHull, convexHull);
								cv::RotatedRect box = cv::fitEllipse(convexHull);

								cv::ellipse(colorFrame, box, cv::Scalar(0, 255, 255), 1, cv::LINE_AA);

								// Draw points of interest for calculating the mouth opening
								cv::Point mouthUpperlipMidbottom = cv::Point(static_cast<int>(verticesScreenSpace.at(1072).X), static_cast<int>(verticesScreenSpace.at(1072).Y));
								cv::Point mouthLowerlipMidtop = cv::Point(static_cast<int>(verticesScreenSpace.at(10).X), static_cast<int>(verticesScreenSpace.at(10).Y));
								cv::circle(colorFrame, mouthUpperlipMidbottom, 3, cv::Scalar(0, 255, 0), -1, CV_AA);
								cv::circle(colorFrame, mouthLowerlipMidtop, 3, cv::Scalar(0, 255, 0), -1, CV_AA);
								cv::line(colorFrame, mouthUpperlipMidbottom, mouthLowerlipMidtop, cv::Scalar(128, 128, 0), 3, CV_AA);

								// Draw points of lower face
								cv::Point chinCenter = cv::Point(static_cast<int>(verticesScreenSpace.at(4).X), static_cast<int>(verticesScreenSpace.at(4).Y));
								cv::Point lowerJawLeftEnd = cv::Point(static_cast<int>(verticesScreenSpace.at(1307).X), static_cast<int>(verticesScreenSpace.at(1307).Y));
								cv::Point lowerJawRightEnd = cv::Point(static_cast<int>(verticesScreenSpace.at(1327).X), static_cast<int>(verticesScreenSpace.at(1327).Y));
								cv::circle(colorFrame, chinCenter, 3, cv::Scalar(0, 100, 100), -1, CV_AA);
								cv::circle(colorFrame, lowerJawLeftEnd, 3, cv::Scalar(0, 100, 100), -1, CV_AA);
								cv::circle(colorFrame, lowerJawRightEnd, 3, cv::Scalar(0, 100, 100), -1, CV_AA);
							}
						}
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
        return HDFaceTracking().Run();
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