#pragma once

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

class KinectDataManager
{
public:
    // Singleton Pattern 
    // - 1. Return static instance
    // - 2. Delete copy constructor and assignment operator in order to make sure no accidental copies are created
    // - 3. Automatically deletes implicit move constructor and move assignment operator!
    static KinectDataManager& GetInstance()
    {
        static KinectDataManager instance;
        return instance;
    }
    KinectDataManager(const KinectDataManager&) = delete;
    void operator=(const KinectDataManager&) = delete;

    // Public member functions
    auto InitializeKinectSensor(const std::shared_ptr<DeviceOptions>& deviceOptions) -> HRESULT;
    auto CloseKinectSensor()                                                         -> HRESULT;
    auto Update()                                                                    -> HRESULT;

    auto GetCoordinateMapper()          const -> ICoordinateMapper*;
    auto GetDeviceOptions()             const -> std::shared_ptr<DeviceOptions>;
	auto GetColorFrame()                const -> const cv::Mat&;
	auto GetDepthFrame()                const -> const cv::Mat&;
	auto GetInfraredFrame()             const -> const cv::Mat&;
	auto GetLongExposureInfraredFrame() const -> const cv::Mat&;
	auto GetBodyIndexFrame()            const -> const cv::Mat&;
	auto GetBodies()                    const -> const std::vector<std::shared_ptr<Body>>&;
	auto GetPrimaryUser()               const -> const std::shared_ptr<Body>;

private:
    // Singleton Pattern - Private contructor
    KinectDataManager() {};

    // Private member functions
    auto GetUnqiueKinectSensorID() const -> std::string;

    // Member variables
    bool m_kinectSensorIsInitialized{false};
    IKinectSensor* m_kinectSensor{nullptr};
    ICoordinateMapper* m_coordinateMapper{nullptr};
    IMultiSourceFrameReader* m_kinectMultiSourceFrameReader{nullptr};

    IFaceFrameSource* m_faceFrameSources[BODY_COUNT];
    IFaceFrameReader* m_faceFrameReaders[BODY_COUNT];

	IHighDefinitionFaceFrameSource* m_highDefinitionFaceFrameSources[BODY_COUNT];
	IHighDefinitionFaceFrameReader* m_highDefinitionFaceFrameReaders[BODY_COUNT];
	IFaceAlignment* m_faceAlignments[BODY_COUNT];
	IFaceModel* m_faceModels[BODY_COUNT];
	std::vector<std::vector<float>> deformations{ BODY_COUNT, std::vector<float>(FaceShapeDeformations::FaceShapeDeformations_Count) };
	UINT32 m_vertexCountHDFace{ 0 };

    std::shared_ptr<DeviceOptions> m_deviceOptions;

	cv::Mat m_colorFrame;
	cv::Mat m_depthFrame;
	cv::Mat m_infraredFrame;
	cv::Mat m_longExposureInfraredFrame;
	cv::Mat m_bodyIndexFrame;

	std::vector<std::shared_ptr<Body>> m_bodies;
	std::shared_ptr<Body> m_primaryUser{ nullptr };

	// Constants
	const std::array<cv::Vec3b, 6> BODY_COLORS = { { cv::Vec3b(255, 0, 0), cv::Vec3b(0, 255, 0), cv::Vec3b(0, 0, 255), cv::Vec3b(255, 255, 0), cv::Vec3b(255, 0, 255), cv::Vec3b(0, 255, 255) } };
};

