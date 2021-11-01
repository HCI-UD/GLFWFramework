#include "CommonKinect.h"

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

// *****************************************************************
// ********************* Init & Close Sensor ***********************
// *****************************************************************

auto KinectDataManager::InitializeKinectSensor(const std::shared_ptr<DeviceOptions>& deviceOptions) -> HRESULT
{
    HRESULT hr{S_OK};

    m_deviceOptions = deviceOptions;

	if (!m_kinectSensorIsInitialized)
	{
		// Get Default KinectSensor
		hr = GetDefaultKinectSensor(&m_kinectSensor);
		if (FAILED(hr))
		{
			return hr;
		}

		if (m_kinectSensor)
		{
			// Open KinectSensor
			hr = m_kinectSensor->Open();
			if (FAILED(hr))
			{
				return hr;
			}

			// Check if sensor was opened successfully
			BOOLEAN isOpen{ false };
			hr = m_kinectSensor->get_IsOpen(&isOpen);

			if (FAILED(hr))
			{
				return hr;
			}

			if (!isOpen)
			{
				hr = E_FAIL;
				return hr;
			}

			// Get Kinect CoordinateMapper
			hr = m_kinectSensor->get_CoordinateMapper(&m_coordinateMapper);
			if (FAILED(hr))
			{
				return hr;
			}

			// Get Kinect MultiSourceFrameReader
			DWORD flags{ 0L };
			if (m_deviceOptions->IsColorEnabled())
				flags |= FrameSourceTypes_Color; // 1
			if (m_deviceOptions->IsInfraredEnabled())
				flags |= FrameSourceTypes_Infrared; // 2
			if (m_deviceOptions->IsLongExposureInfraredEnabled())
				flags |= FrameSourceTypes_LongExposureInfrared; // 4
			if (m_deviceOptions->IsDepthEnabled())
				flags |= FrameSourceTypes_Depth; // 8
			if (m_deviceOptions->IsBodyIndexEnabled())
				flags |= FrameSourceTypes_BodyIndex; // 16
			if (m_deviceOptions->IsBodyEnabled())
				flags |= FrameSourceTypes_Body; // 32
			if (m_deviceOptions->IsAudioEnabled())
				flags |= FrameSourceTypes_Audio; // 64
			hr = m_kinectSensor->OpenMultiSourceFrameReader(flags, &m_kinectMultiSourceFrameReader);
			if (FAILED(hr))
			{
				return hr;
			}

			/*
			 * Acquire the dimensions of all different frame sources (i.e. color,
			 * infrared, long exposure infrared, depth, and body index) and store
			 * values as member variables for later usage.
			 */
#pragma region ACQUIRE_FRAME_DIMENSIONS

			 // Color
#pragma region ACQUIRE_COLOR_DIMENSIONS
			if (m_deviceOptions->IsColorEnabled())
			{
				IColorFrameSource* colorFrameSource{ nullptr };
				hr = m_kinectSensor->get_ColorFrameSource(&colorFrameSource);

				if (SUCCEEDED(hr))
				{
					IFrameDescription* colorFrameDescription{ nullptr };
					hr = colorFrameSource->get_FrameDescription(&colorFrameDescription);

					if (SUCCEEDED(hr))
					{
						int32_t colorWidth{ 0 };
						int32_t colorHeight{ 0 };

						hr = colorFrameDescription->get_Width(&colorWidth);
						hr = colorFrameDescription->get_Height(&colorHeight);

						if (SUCCEEDED(hr))
						{
							m_colorFrame = cv::Mat::zeros(cv::Size(colorWidth, colorHeight), CV_8UC3);
						}
					}
				}
			}
#pragma endregion ACQUIRE_COLOR_DIMENSIONS

			// Infrared
#pragma region ACQUIRE_INFRARED_DIMENSIONS
			if (m_deviceOptions->IsInfraredEnabled())
			{
				IInfraredFrameSource* infraredFrameSource{ nullptr };
				m_kinectSensor->get_InfraredFrameSource(&infraredFrameSource);

				if (SUCCEEDED(hr))
				{
					IFrameDescription* infraredFrameDescription{ nullptr };
					hr = infraredFrameSource->get_FrameDescription(&infraredFrameDescription);

					if (SUCCEEDED(hr))
					{
						int32_t infraredWidth{ 0 };
						int32_t infaredHeight{ 0 };

						hr = infraredFrameDescription->get_Width(&infraredWidth);
						hr = infraredFrameDescription->get_Height(&infaredHeight);

						if (SUCCEEDED(hr))
						{
							m_infraredFrame = cv::Mat::zeros(cv::Size(infraredWidth, infaredHeight), CV_16UC1);
						}
					}
				}
			}
#pragma endregion ACQUIRE_INFRARED_DIMENSIONS

			// Long Exposure Infrared
#pragma region ACQUIRE_LONGEXPOSUREINFRARED_DIMENSIONS
			if (m_deviceOptions->IsLongExposureInfraredEnabled())
			{
				ILongExposureInfraredFrameSource* longExposureInfraredFrameSource{ nullptr };
				m_kinectSensor->get_LongExposureInfraredFrameSource(&longExposureInfraredFrameSource);

				if (SUCCEEDED(hr))
				{
					IFrameDescription* longExposureInfraredFrameDescription{ nullptr };
					hr = longExposureInfraredFrameSource->get_FrameDescription(&longExposureInfraredFrameDescription);

					if (SUCCEEDED(hr))
					{
						int32_t longExposureInfraredWidth{ 0 };
						int32_t longExposureInfaredHeight{ 0 };

						hr = longExposureInfraredFrameDescription->get_Width(&longExposureInfraredWidth);
						hr = longExposureInfraredFrameDescription->get_Height(&longExposureInfaredHeight);

						if (SUCCEEDED(hr))
						{
							m_longExposureInfraredFrame = cv::Mat::zeros(cv::Size(longExposureInfraredWidth, longExposureInfaredHeight), CV_16UC1);
						}
					}
				}
			}
#pragma endregion ACQUIRE_LONGEXPOSUREINFRARED_DIMENSIONS

			// Depth
#pragma region ACQUIRE_DEPTH_DIMENSIONS
			if (m_deviceOptions->IsDepthEnabled())
			{
				IDepthFrameSource* depthFrameSource{ nullptr };
				m_kinectSensor->get_DepthFrameSource(&depthFrameSource);

				if (SUCCEEDED(hr))
				{
					IFrameDescription* depthFrameDescription{ nullptr };
					hr = depthFrameSource->get_FrameDescription(&depthFrameDescription);

					if (SUCCEEDED(hr))
					{
						int32_t depthWidth{ 0 };
						int32_t depthHeight{ 0 };

						hr = depthFrameDescription->get_Width(&depthWidth);
						hr = depthFrameDescription->get_Height(&depthHeight);

						if (SUCCEEDED(hr))
						{
							m_depthFrame = cv::Mat::zeros(cv::Size(depthWidth, depthHeight), CV_16UC1);
						}
					}
				}
			}
#pragma endregion ACQUIRE_DEPTH_DIMENSIONS

			// Body Index Frame
#pragma region ACQUIRE_BODYINDEX_DIMENSIONS
			if (m_deviceOptions->IsBodyIndexEnabled())
			{
				IBodyIndexFrameSource* bodyIndexFrameSource{ nullptr };
				m_kinectSensor->get_BodyIndexFrameSource(&bodyIndexFrameSource);

				if (SUCCEEDED(hr))
				{
					IFrameDescription* bodyIndexFrameDescription{ nullptr };
					hr = bodyIndexFrameSource->get_FrameDescription(&bodyIndexFrameDescription);

					if (SUCCEEDED(hr))
					{
						int32_t bodyIndexWidth{ 0 };
						int32_t bodyIndexHeight{ 0 };

						hr = bodyIndexFrameDescription->get_Width(&bodyIndexWidth);
						hr = bodyIndexFrameDescription->get_Height(&bodyIndexHeight);

						if (SUCCEEDED(hr))
						{
							m_bodyIndexFrame = cv::Mat::zeros(cv::Size(bodyIndexWidth, bodyIndexHeight), CV_8UC3);
						}
					}
				}
			}
#pragma endregion ACQUIRE_BODYINDEX_DIMENSIONS
#pragma endregion ACQUIRE_FRAME_DIMENSIONS

			// Initialize Face Tracking API
			if (m_deviceOptions->IsBodyEnabled() && m_deviceOptions->IsFaceEnabled())
			{
				// Face Tracking Features
				DWORD features = FaceFrameFeatures_BoundingBoxInColorSpace
					| FaceFrameFeatures_PointsInColorSpace
					| FaceFrameFeatures_RotationOrientation
					| FaceFrameFeatures_Happy
					| FaceFrameFeatures_RightEyeClosed
					| FaceFrameFeatures_LeftEyeClosed
					| FaceFrameFeatures_MouthOpen
					| FaceFrameFeatures_MouthMoved
					| FaceFrameFeatures_LookingAway
					| FaceFrameFeatures_Glasses
					| FaceFrameFeatures_FaceEngagement;

				// For each body, create FaceFrameSource and open an associated FaceFrameReader
				for (auto count = 0; count < BODY_COUNT; ++count)
				{
					hr = CreateFaceFrameSource(m_kinectSensor, 0, features, &m_faceFrameSources[count]);
					if (FAILED(hr))
					{
						return hr;
					}

					hr = m_faceFrameSources[count]->OpenReader(&m_faceFrameReaders[count]);
					if (FAILED(hr))
					{
						return hr;
					}
				}
			}

			// Initialize HD Face Tracking API
			if (m_deviceOptions->IsBodyEnabled() && m_deviceOptions->IsHDFaceEnabled())
			{
				hr = GetFaceModelVertexCount(&m_vertexCountHDFace);
				if (FAILED(hr))
				{
					return hr;
				}

				for (auto count = 0; count < BODY_COUNT; ++count)
				{
					hr = CreateHighDefinitionFaceFrameSource(m_kinectSensor, &m_highDefinitionFaceFrameSources[count]);
					if (FAILED(hr))
					{
						return hr;
					}

					hr = m_highDefinitionFaceFrameSources[count]->OpenReader(&m_highDefinitionFaceFrameReaders[count]);
					if (FAILED(hr))
					{
						return hr;
					}

					hr = CreateFaceAlignment(&m_faceAlignments[count]);
					if (FAILED(hr))
					{
						return hr;
					}

					hr = CreateFaceModel(1.0f, FaceShapeDeformations::FaceShapeDeformations_Count, &deformations[count][0], &m_faceModels[count]);
					if (FAILED(hr))
					{
						return hr;
					}
				}
			}

			// Retrieve Kinect UID
			std::string deviceUID = this->GetUnqiueKinectSensorID();
			std::cout << "Unique Kinect ID: " << deviceUID << std::endl;
			m_deviceOptions->SetDeviceUID(deviceUID);

			/*
			* Check if sensor is available (i.e. connected)
			*
			* According to the Kinect SDK, this has to be called
			* after opening the Kinect sensor. However, directly
			* calling get_isAvailable after opening the sensor
			* yields false, so apparently the Kinect sensor needs
			* some time to initialize. Therefore, get_isAvailable
			* is called after retrieving the unique sensor ID.
			*/
			BOOLEAN isAvailable{ false };
			hr = m_kinectSensor->get_IsAvailable(&isAvailable);

			if (FAILED(hr))
			{
				return hr;
			}

			if (!isAvailable)
			{
				hr = E_FAIL;
				return hr;
			}

			// Initialization successful
			m_kinectSensorIsInitialized = true;
		}
	}

    return hr;
}

auto KinectDataManager::CloseKinectSensor() -> HRESULT
{
    HRESULT hr{S_OK};

    SafeRelease(m_coordinateMapper);
    SafeRelease(m_kinectMultiSourceFrameReader);

	if (m_deviceOptions->IsBodyEnabled() && m_deviceOptions->IsFaceEnabled())
	{
		for (int count = 0; count < BODY_COUNT; count++)
		{
			SafeRelease(m_faceFrameSources[count]);
			SafeRelease(m_faceFrameReaders[count]);
		}
	}

	if (m_deviceOptions->IsBodyEnabled() && m_deviceOptions->IsHDFaceEnabled())
	{
		for (int count = 0; count < BODY_COUNT; count++)
		{
			SafeRelease(m_highDefinitionFaceFrameSources[count]);
			SafeRelease(m_highDefinitionFaceFrameReaders[count]);
			SafeRelease(m_faceAlignments[count]);
			SafeRelease(m_faceModels[count]);
		}
	}

    if (m_kinectSensor)
    {
        hr = m_kinectSensor->Close();

        if (FAILED(hr))
        {
            hr = E_FAIL;
        }
    }
    SafeRelease(m_kinectSensor);

    return hr;
}


// ****************************************************
// ********************* Update ***********************
// ****************************************************

auto KinectDataManager::Update() -> HRESULT
{
    HRESULT hr{S_OK};

    static int frameCounter{0};
	frameCounter++;

    IMultiSourceFrame* multiSourceFrame{nullptr};
    hr = m_kinectMultiSourceFrameReader->AcquireLatestFrame(&multiSourceFrame);

    if (SUCCEEDED(hr))
    {
        /*
         * Acquire individual frames for the sources enabled in the DeviceOptions (i.e. Color, Infrared, 
		 * Long Exposure Infrared, Body Index, and Body) by means of the multiSourceFrame variable. The 
         * underlying buffers are accessed and the data is copied to a cv::Mat member variable.
		 */

        // Color
        if (m_deviceOptions->IsColorEnabled())
        {
            IColorFrameReference* colorFrameReference{nullptr};
            hr = multiSourceFrame->get_ColorFrameReference(&colorFrameReference);
            if (SUCCEEDED((hr)))
            {
				IColorFrame* colorFrame{ nullptr };
                hr = colorFrameReference->AcquireFrame(&colorFrame);
				if (SUCCEEDED(hr))
				{
					cv::Mat colorMat = cv::Mat::zeros(cv::Size(m_colorFrame.cols, m_colorFrame.rows), CV_8UC4);
					unsigned int colorBufferSize = m_colorFrame.rows * m_colorFrame.cols * sizeof(unsigned char) * 4;
					hr = colorFrame->CopyConvertedFrameDataToArray(colorBufferSize, reinterpret_cast<unsigned char*>(colorMat.data), ColorImageFormat_Bgra);

					if (SUCCEEDED(hr))
					{
						cv::cvtColor(colorMat, colorMat, CV_BGRA2BGR);
						m_colorFrame = colorMat;
					}
				}
				SafeRelease(colorFrame);
            }
            SafeRelease(colorFrameReference);
        }

        // Infrared
        if (m_deviceOptions->IsInfraredEnabled())
        {
            IInfraredFrameReference* infraredFrameReference{nullptr};
            hr = multiSourceFrame->get_InfraredFrameReference(&infraredFrameReference);
            if (SUCCEEDED((hr)))
            {
				IInfraredFrame* infraredFrame{ nullptr };
                hr = infraredFrameReference->AcquireFrame(&infraredFrame);
				if (SUCCEEDED(hr))
				{
					cv::Mat infraredMat = cv::Mat::zeros(cv::Size(m_infraredFrame.cols, m_infraredFrame.rows), CV_16UC1);
					unsigned int infraredBufferSize{ 0 };
					hr = infraredFrame->AccessUnderlyingBuffer(&infraredBufferSize, reinterpret_cast<unsigned short**>(infraredMat.data));

					if (SUCCEEDED(hr))
					{
						m_infraredFrame = infraredMat;
					}
				}
				SafeRelease(infraredFrame);
            }
            SafeRelease(infraredFrameReference);
        }

        // Long Exposure Infrared
        if (m_deviceOptions->IsLongExposureInfraredEnabled())
        {
            ILongExposureInfraredFrameReference* longExposureInfraredFrameReference{nullptr};
            hr = multiSourceFrame->get_LongExposureInfraredFrameReference(&longExposureInfraredFrameReference);
            if (SUCCEEDED((hr)))
            {
				ILongExposureInfraredFrame* longExposureInfraredFrame{ nullptr };
                hr = longExposureInfraredFrameReference->AcquireFrame(&longExposureInfraredFrame);
				if (SUCCEEDED(hr))
				{
					cv::Mat longExposureInfraredMat = cv::Mat::zeros(cv::Size(m_longExposureInfraredFrame.cols, m_longExposureInfraredFrame.rows), CV_16UC1);
					unsigned int longExposureInfraredBufferSize{ 0 };
					hr = longExposureInfraredFrame->AccessUnderlyingBuffer(&longExposureInfraredBufferSize, reinterpret_cast<unsigned short**>(longExposureInfraredMat.data));

					if (SUCCEEDED(hr))
					{
						m_longExposureInfraredFrame = longExposureInfraredMat;
					}
				}
				SafeRelease(longExposureInfraredFrame);
            }
            SafeRelease(longExposureInfraredFrameReference);
        }

        // Depth
        if (m_deviceOptions->IsDepthEnabled())
        {
            IDepthFrameReference* depthFrameReference{nullptr};
            hr = multiSourceFrame->get_DepthFrameReference(&depthFrameReference);
            if (SUCCEEDED((hr)))
            {
				IDepthFrame* depthFrame{ nullptr };
                hr = depthFrameReference->AcquireFrame(&depthFrame);
				if (SUCCEEDED(hr))
				{
					cv::Mat depthMat = cv::Mat::zeros(cv::Size(m_depthFrame.cols, m_depthFrame.rows), CV_16UC1);
					unsigned int depthBufferSize{ 0 };
					hr = depthFrame->AccessUnderlyingBuffer(&depthBufferSize, reinterpret_cast<unsigned short**>(depthMat.data));

					if (SUCCEEDED(hr))
					{
						m_depthFrame = depthMat;
					}
				}
				SafeRelease(depthFrame);
            }
            SafeRelease(depthFrameReference);
        }

        // Body Index
        if (m_deviceOptions->IsBodyIndexEnabled())
        {
            IBodyIndexFrameReference* bodyIndexFrameReference{nullptr};
            hr = multiSourceFrame->get_BodyIndexFrameReference(&bodyIndexFrameReference);
            if (SUCCEEDED((hr)))
            {
				IBodyIndexFrame* bodyIndexFrame{ nullptr };
                hr = bodyIndexFrameReference->AcquireFrame(&bodyIndexFrame);
				if (SUCCEEDED(hr))
				{
					cv::Mat bodyIndexMat = cv::Mat::zeros(cv::Size(m_bodyIndexFrame.cols, m_bodyIndexFrame.rows), CV_8UC3);
					unsigned int bodyIndexBufferSize{ 0 };
					unsigned char* bodyIndexBuffer{ nullptr };
					hr = bodyIndexFrame->AccessUnderlyingBuffer(&bodyIndexBufferSize, &bodyIndexBuffer);

					if (SUCCEEDED(hr))
					{
						// Loop over frame data and assign color to body pixels
						for (int y = 0; y < m_bodyIndexFrame.cols; ++y)
						{
							for (int x = 0; x < m_bodyIndexFrame.rows; ++x)
							{
								unsigned int index = y *  m_bodyIndexFrame.rows + x;

								if (bodyIndexBuffer[index] != 0xff)
								{
									bodyIndexMat.at<cv::Vec3b>(y, x) = BODY_COLORS[bodyIndexBuffer[index]];
								}
								else
								{
									bodyIndexMat.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
								}
							}
						}

						m_bodyIndexFrame = bodyIndexMat;
					}
				}
				SafeRelease(bodyIndexFrame);
            }
            SafeRelease(bodyIndexFrameReference);
        }

        // Body + Face + HDFace
        if (m_deviceOptions->IsBodyEnabled())
        {
            IBodyFrameReference* bodyFrameReference{nullptr};
            hr = multiSourceFrame->get_BodyFrameReference(&bodyFrameReference);
            if (SUCCEEDED((hr)))
            {
				IBodyFrame* bodyFrame{ nullptr };
                hr = bodyFrameReference->AcquireFrame(&bodyFrame);
				if (SUCCEEDED(hr))
				{
					IBody* bodies[BODY_COUNT]{ nullptr };
					hr = bodyFrame->GetAndRefreshBodyData(BODY_COUNT, bodies);

					if (SUCCEEDED(hr))
					{
						/*
						 * As a first step, the IDs of all lost bodies have to be retrieved in 
						 * order to remove the associated Body from the m_bodies vector. This 
						 * is achieved by copying all IDs from m_bodies to a vector lostBodyIDs
						 * and subsequently removing those which were already present in the 
						 * previous frame.
						 */
						std::vector<uint64_t> lostBodyIDs;
						for (const auto& b : m_bodies)
						{
							lostBodyIDs.push_back(b->GetID());
						}

						for (int currentBodyIndex = 0; currentBodyIndex < BODY_COUNT; ++currentBodyIndex)
						{
							IBody* currentBody = bodies[currentBodyIndex];

							BOOLEAN currentBodyIsTracked{ false };
							hr = currentBody->get_IsTracked(&currentBodyIsTracked);

							if (SUCCEEDED(hr) && currentBodyIsTracked)
							{
								uint64_t trackingID{ 0 };
								hr = currentBody->get_TrackingId(&trackingID);

								if (SUCCEEDED(hr))
								{
									if (std::find(lostBodyIDs.begin(), lostBodyIDs.end(), trackingID) != lostBodyIDs.end())
									{
										lostBodyIDs.erase(std::remove(lostBodyIDs.begin(), lostBodyIDs.end(), trackingID), lostBodyIDs.end());
									}
								}
							}
						}

						/*
						 * Based on the lostBodyIDs vector calculated in the previous step,
						 * remove those bodies from the m_bodies vector which were not 
						 * present in the previous frame. For these bodies, a new Body object
						 * is created while information of all other bodies will just be updated.
						 */
						for (const int64_t lostUserID : lostBodyIDs)
						{
							m_bodies.erase(std::remove_if(m_bodies.begin(), m_bodies.end(), [lostUserID](std::shared_ptr<Body> body) { return body->GetID() == lostUserID; }), m_bodies.end());
							std::cout << "LOST BODY: " << lostUserID << std::endl;
						}

						/*
						 * Loop over all bodies detected in this frame and update those that
						 * were present in the previous frame, and create a new Body object
						 * for all newly detected bodies.
						 */
						for (int currentBodyIndex = 0; currentBodyIndex < BODY_COUNT; ++currentBodyIndex)
						{
							IBody* currentBody = bodies[currentBodyIndex];

							BOOLEAN currentBodyIsTracked{ false };
							hr = currentBody->get_IsTracked(&currentBodyIsTracked);

							if (SUCCEEDED(hr) && currentBodyIsTracked)
							{
								uint64_t trackingID{ 0 };
								hr = currentBody->get_TrackingId(&trackingID);

								// Acquire Joint Data
								#pragma region ACQUIRE_JOINT_DATA
								std::map<JointType, Body::BodyJoint> jointMap;
								if (SUCCEEDED(hr))
								{
									Joint joints[JointType_Count];
									hr = currentBody->GetJoints(JointType_Count, joints);

									JointOrientation jointOrientations[JointType_Count];
									hr = currentBody->GetJointOrientations(JointType_Count, jointOrientations);

									if (SUCCEEDED(hr))
									{
										// Loop over all joints
										for (int currentJointIndex = 0; currentJointIndex < JointType_Count; ++currentJointIndex)
										{
											Joint currentJoint = joints[currentJointIndex];
											Body::BodyJoint bodyJoint(currentJoint.Position, jointOrientations[currentJointIndex].Orientation, currentJoint.TrackingState);
											jointMap.insert(std::pair<JointType, Body::BodyJoint>(currentJoint.JointType, bodyJoint));
										}
									}
								}
								#pragma endregion ACQUIRE_JOINT_DATA

								// Acquire Additional Body Data
								#pragma region ACQUIRE_ADDITIONAL_BODY_DATA
								HandState leftHandState{ HandState_Unknown };
								TrackingConfidence leftHandConfidence{ TrackingConfidence_Low };
								HandState rightHandState{ HandState_Unknown };
								TrackingConfidence rightHandConfidence{ TrackingConfidence_Low };

								hr = currentBody->get_HandLeftState(&leftHandState);
								hr = currentBody->get_HandLeftConfidence(&leftHandConfidence);
								hr = currentBody->get_HandRightState(&rightHandState);
								hr = currentBody->get_HandRightConfidence(&rightHandConfidence);

								// TODO: FUTURE IDEAS
								// - Activity (EyeClosed, MouthOpen, LookingAway)
								// - Appearance (WearingGlasses)
								// - Expression (Neutral, Happy)
								// - Lean (between -1 and 1)
								#pragma endregion ACQUIRE_ADDITIONAL_BODY_DATA

								// Acquire Face Data
								#pragma region FACE_TRACKING
								std::shared_ptr<Body::BodyFace> face{ nullptr };
								if (SUCCEEDED(hr) && m_deviceOptions->IsFaceEnabled())
								{
									hr = m_faceFrameSources[currentBodyIndex]->put_TrackingId(trackingID);

									if (SUCCEEDED(hr))
									{
										IFaceFrame* faceFrames[BODY_COUNT]{ nullptr };
										hr = m_faceFrameReaders[currentBodyIndex]->AcquireLatestFrame(&faceFrames[currentBodyIndex]);
										IFaceFrame* currentFaceFrame = faceFrames[currentBodyIndex];

										if (SUCCEEDED(hr) && currentFaceFrame != nullptr)
										{
											BOOLEAN faceTracked{ false };
											hr = currentFaceFrame->get_IsTrackingIdValid(&faceTracked);

											if (SUCCEEDED(hr) && faceTracked)
											{
												IFaceFrameResult* faceFrameResult{ nullptr };
												hr = currentFaceFrame->get_FaceFrameResult(&faceFrameResult);

												if (SUCCEEDED(hr) && faceFrameResult != nullptr)
												{
													face = std::make_shared<Body::BodyFace>();

													// Face Bounding Box
													RectI boundingBox = RectI{ 0, 0, 1, 1 };
													hr = faceFrameResult->get_FaceBoundingBoxInColorSpace(&boundingBox);
													if (SUCCEEDED(hr))
													{
														face->SetBoundingBoxInColorSpace(boundingBox);
													}

													// Face Points of Interest (Eyes, Nose, Mouth)
													PointF facePointsOfInterest[FacePointType::FacePointType_Count];
													hr = faceFrameResult->GetFacePointsInColorSpace(FacePointType::FacePointType_Count, facePointsOfInterest);
													if (SUCCEEDED(hr))
													{
														face->SetPositionLeftEyeInColorSpace(cv::Point(static_cast<int>(facePointsOfInterest[0].X), static_cast<int>(facePointsOfInterest[0].Y)));
														face->SetPositionRightEyeInColorSpace(cv::Point(static_cast<int>(facePointsOfInterest[1].X), static_cast<int>(facePointsOfInterest[1].Y)));
														face->SetPositionNoseEyeInColorSpace(cv::Point(static_cast<int>(facePointsOfInterest[2].X), static_cast<int>(facePointsOfInterest[2].Y)));
														face->SetPositionMouthLeftInColorSpace(cv::Point(static_cast<int>(facePointsOfInterest[3].X), static_cast<int>(facePointsOfInterest[3].Y)));
														face->SetPositionMouthRightInColorSpace(cv::Point(static_cast<int>(facePointsOfInterest[4].X), static_cast<int>(facePointsOfInterest[4].Y)));
													}

													// Face Rotation
													Vector4 faceRotation;
													hr = faceFrameResult->get_FaceRotationQuaternion(&faceRotation);
													if (SUCCEEDED(hr))
													{
														face->SetFaceRotation(faceRotation);
													}

													// Face Properties
													DetectionResult faceProperties[FaceProperty::FaceProperty_Count];
													hr = faceFrameResult->GetFaceProperties(FaceProperty::FaceProperty_Count, faceProperties);
													if (SUCCEEDED(hr))
													{
														face->SetDetectionResultHappy(faceProperties[0]);
														face->SetDetectionResultFaceEngangement(faceProperties[1]);
														face->SetDetectionResultWearingGlasses(faceProperties[2]);
														face->SetDetectionResultLeftEyeClosed(faceProperties[3]);
														face->SetDetectionResultRightEyeClosed(faceProperties[4]);
														face->SetDetectionResultMouthOpen(faceProperties[5]);
														face->SetDetectionResultMouthMoved(faceProperties[6]);
														face->SetDetectionResultLookingAway(faceProperties[7]);
													}
												}

												SafeRelease(faceFrameResult);
											}
										}

										for (auto count = 0; count < BODY_COUNT; ++count)
										{
											SafeRelease(faceFrames[count]);
										}
									}
								}
#pragma endregion FACE_TRACKING

								// Acquire HD Face Data
								#pragma region HD_FACE_TRACKING
								std::shared_ptr<Body::BodyHDFace> hdFace{ nullptr };
								if (SUCCEEDED(hr) && m_deviceOptions->IsHDFaceEnabled())
								{
									hr = m_highDefinitionFaceFrameSources[currentBodyIndex]->put_TrackingId(trackingID);

									if (SUCCEEDED(hr))
									{
										IHighDefinitionFaceFrame* highDefinitionFaceFrames[BODY_COUNT]{ nullptr };
										hr = m_highDefinitionFaceFrameReaders[currentBodyIndex]->AcquireLatestFrame(&highDefinitionFaceFrames[currentBodyIndex]);
										IHighDefinitionFaceFrame* currentHighDefinitionFaceFrame = highDefinitionFaceFrames[currentBodyIndex];

										if (SUCCEEDED(hr) && currentHighDefinitionFaceFrame != nullptr)
										{
											BOOLEAN highDefinitionFaceTracked{ false };
											hr = currentHighDefinitionFaceFrame->get_IsFaceTracked(&highDefinitionFaceTracked);

											if (SUCCEEDED(hr) && highDefinitionFaceTracked)
											{
												hr = currentHighDefinitionFaceFrame->GetAndRefreshFaceAlignmentResult(m_faceAlignments[currentBodyIndex]);
												IFaceAlignment* currentFaceAlignment = m_faceAlignments[currentBodyIndex];

												if (SUCCEEDED(hr) && currentFaceAlignment != nullptr)
												{
													hdFace = std::make_shared<Body::BodyHDFace>();

													RectI hdFaceBoundingBoxInColorSpace = RectI{ 0, 0, 1, 1 };
													hr = currentFaceAlignment->get_FaceBoundingBox(&hdFaceBoundingBoxInColorSpace);
													if (SUCCEEDED(hr))
													{
														hdFace->SetBoundingBoxInColorSpace(hdFaceBoundingBoxInColorSpace);
													}

													Vector4 hdFaceOrientation = Vector4{ 0.0f, 0.0f, 0.0f, 0.0f };
													hr = currentFaceAlignment->get_FaceOrientation(&hdFaceOrientation);
													if (SUCCEEDED(hr))
													{
														hdFace->SetFaceRotation(hdFaceOrientation);
													}

													CameraSpacePoint hdFacePivotPoint = CameraSpacePoint{ 0.0f, 0.0f, 0.0f };
													hr = currentFaceAlignment->get_HeadPivotPoint(&hdFacePivotPoint);
													if (SUCCEEDED(hr))
													{
														hdFace->SetHeadPivotWorldPosition(hdFacePivotPoint);
													}

													FaceAlignmentQuality hdFaceAlignmentQuality{ FaceAlignmentQuality::FaceAlignmentQuality_Low };
													hr = currentFaceAlignment->get_Quality(&hdFaceAlignmentQuality);
													if (SUCCEEDED(hr))
													{
														hdFace->SetFaceAlignmentQuality(hdFaceAlignmentQuality);
													}

													// HD Face Model Vertices
													IFaceModel* currentFaceModel = m_faceModels[currentBodyIndex];

													if (currentFaceModel != nullptr)
													{
														std::vector<CameraSpacePoint> verticesWorldSpace(m_vertexCountHDFace);
														hr = currentFaceModel->CalculateVerticesForAlignment(m_faceAlignments[currentBodyIndex], m_vertexCountHDFace, &verticesWorldSpace[0]);
														if (SUCCEEDED(hr))
														{
															hdFace->SetVerticesWorldSpace(verticesWorldSpace);
														}
													}
												}
											}
										}
										for (auto count = 0; count < BODY_COUNT; ++count)
										{
											SafeRelease(highDefinitionFaceFrames[count]);
										}
									}
								}
								#pragma endregion HD_FACE_TRACKING

								/*
								 * Check whether the m_bodies vector already contains a Body corresponding
								 * to the trackingID of the currentBody. If so, update the body information.
								 * Otherwise, create a new body object and add it to m_bodies.
								 */
								auto index = std::find_if(m_bodies.begin(), m_bodies.end(), [trackingID](std::shared_ptr<Body> body) { return body->GetID() == trackingID; });

								std::shared_ptr<Body> body;

								if (index != std::end(m_bodies))
								{
									body = m_bodies.at(std::distance(std::begin(m_bodies), index));
									body->SetJointMap(jointMap);
									body->SetLeftHandState(leftHandState);
									body->SetRightHandState(rightHandState);
									body->SetLeftHandConfidence(leftHandConfidence);
									body->SetRightHandConfidence(rightHandConfidence);
									body->SetBodyFace(face);
									body->SetBodyHDFace(hdFace);
								}
								else
								{
									std::cout << "NEW BODY DETECTED: " << trackingID << std::endl;
									body = std::make_shared<Body>(trackingID, jointMap, leftHandState, rightHandState, leftHandConfidence, rightHandConfidence, face, hdFace);
									m_bodies.push_back(body);
								}

								/*
								 * Check whether current body is the primary user, i.e. the body
								 * closest to the Kinect sensor, by comparing the absolute distance
								 * to the head joint.
								 */
								if (m_primaryUser == nullptr || std::abs(body->GetJointMap().at(JointType_Head).GetWorldPosition().Z) < std::abs(m_primaryUser->GetJointMap().at(JointType_Head).GetWorldPosition().Z))
								{
									m_primaryUser = body;
								}
							}
						}
					}

					for (int count = 0; count < BODY_COUNT; ++count)
					{
						SafeRelease(bodies[count]);
					}
				}
				SafeRelease(bodyFrame);
            }
            SafeRelease(bodyFrameReference);
        }
    }

    SafeRelease(multiSourceFrame);

    return hr;
}


// ****************************************************
// ********************* Getter ***********************
// ****************************************************

auto KinectDataManager::GetCoordinateMapper() const -> ICoordinateMapper*
{
    return m_coordinateMapper;
}

auto KinectDataManager::GetDeviceOptions() const -> std::shared_ptr<DeviceOptions>
{
    return m_deviceOptions;
}

auto KinectDataManager::GetColorFrame() const -> const cv::Mat&
{
	return m_colorFrame;
}

auto KinectDataManager::GetDepthFrame() const -> const cv::Mat&
{
	return m_depthFrame;
}

auto KinectDataManager::GetInfraredFrame() const -> const cv::Mat&
{
	return m_infraredFrame;
}

auto KinectDataManager::GetLongExposureInfraredFrame() const -> const cv::Mat&
{
	return m_longExposureInfraredFrame;
}

auto KinectDataManager::GetBodyIndexFrame() const -> const cv::Mat&
{
	return m_bodyIndexFrame;
}

auto KinectDataManager::GetBodies() const -> const std::vector<std::shared_ptr<Body>>&
{
	return m_bodies;
}

auto KinectDataManager::GetPrimaryUser() const -> const std::shared_ptr<Body>
{
	return m_primaryUser;
}

// **********************************************************************
// ********************* Private Member Functions ***********************
// **********************************************************************

/*
 * Returns the unique ID of the Kinect sensor.
 *
 * Notes: - Needs quite a bit of time until the sensor has been initialized (hence the ::Sleep call).
 *        - Can only be called after Kinect sensor has been opened.
 */
auto KinectDataManager::GetUnqiueKinectSensorID() const -> std::string
{
    unsigned int maxAttempts = 5;

    for (unsigned int currentAttempt = 0; currentAttempt < maxAttempts; ++currentAttempt)
    {
        std::vector<WCHAR> kinectUIDBuffer(256, 0);

		HRESULT hr{ S_OK };
        hr = m_kinectSensor->get_UniqueKinectId(static_cast<UINT>(kinectUIDBuffer.size()), kinectUIDBuffer.data());

        if (hr == S_OK)
        {
            while (!kinectUIDBuffer.empty() && kinectUIDBuffer.back() == 0)
            {
                kinectUIDBuffer.pop_back();
            }

            std::string kinectUID(kinectUIDBuffer.size(), '0');
            for (size_t i = 0; i < kinectUIDBuffer.size(); ++i)
            {
                kinectUID[i] = static_cast<char>(kinectUIDBuffer.at(i));
            }

            if (!kinectUID.empty())
            {
                return kinectUID;
            }
        }

        ::Sleep(1000);
    }

    return "0";
}

