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
// ************************* Body Methods **************************
// *****************************************************************

Body::Body(uint64_t bodyId, const std::map<JointType, BodyJoint>& jointMap, HandState leftHandState, HandState rightHandState, TrackingConfidence leftHandConfidence, TrackingConfidence rightHandConfidence, std::shared_ptr<BodyFace> const& face, std::shared_ptr<BodyHDFace> const& hdFace) :
	m_bodyID(bodyId),
    m_jointMap(jointMap),
    m_leftHandState(leftHandState),
    m_rightHandState(rightHandState),
    m_leftHandConfidence(leftHandConfidence),
    m_rightHandConfidence(rightHandConfidence),
    m_face(face),
	m_hdFace(hdFace)
{
}

Body::~Body()
{
}

auto Body::GetID() const -> const uint64_t
{
    return m_bodyID;
}

auto Body::GetBodyHeight() const -> const float
{
    // Necessary joints for calculating user height
    BodyJoint headJoint = m_jointMap.at(JointType_Head);
    BodyJoint neckJoint = m_jointMap.at(JointType_Neck);
    BodyJoint spineShoulderJoint = m_jointMap.at(JointType_SpineShoulder);
    BodyJoint spineMidJoint = m_jointMap.at(JointType_SpineMid);
    BodyJoint spineBaseJoint = m_jointMap.at(JointType_SpineBase);
    BodyJoint hipLeftJoint = m_jointMap.at(JointType_HipLeft);
    BodyJoint hipRightJoint = m_jointMap.at(JointType_HipRight);
    BodyJoint kneeLeftJoint = m_jointMap.at(JointType_KneeLeft);
    BodyJoint ankleLeftJoint = m_jointMap.at(JointType_AnkleLeft);
    BodyJoint footLeftJoint = m_jointMap.at(JointType_FootLeft);
    BodyJoint kneeRightJoint = m_jointMap.at(JointType_KneeRight);
    BodyJoint ankleRightJoint = m_jointMap.at(JointType_AnkleRight);
    BodyJoint footRightJoint = m_jointMap.at(JointType_FootRight);
    std::array<BodyJoint, 13> userHeightJoints{{headJoint, neckJoint, spineShoulderJoint, spineMidJoint, spineBaseJoint, hipLeftJoint, hipRightJoint, kneeLeftJoint, ankleLeftJoint, footLeftJoint, kneeRightJoint, ankleRightJoint, footRightJoint}};

    // Check tracking state of all joints
    for (const auto& joint : userHeightJoints)
    {
        if (joint.GetTrackingState() != TrackingState_Tracked)
        {
            return 0.0f;
        }
    }

    // Calculate individual heights of body parts
    float heightHeadCenterToTop = 0.07f;
    float heightTorso = CalculateDistanceBetweenJoints(headJoint, neckJoint) + CalculateDistanceBetweenJoints(neckJoint, spineShoulderJoint) + CalculateDistanceBetweenJoints(spineShoulderJoint, spineMidJoint) + CalculateDistanceBetweenJoints(spineMidJoint, spineBaseJoint) + (CalculateDistanceBetweenJoints(spineBaseJoint, hipLeftJoint) + CalculateDistanceBetweenJoints(spineBaseJoint, hipRightJoint)) / 2;
    float heightLeftLeg = CalculateDistanceBetweenJoints(hipLeftJoint, kneeLeftJoint) + CalculateDistanceBetweenJoints(kneeLeftJoint, ankleLeftJoint) + CalculateDistanceBetweenJoints(ankleLeftJoint, footLeftJoint);
    float heightRightLeg = CalculateDistanceBetweenJoints(hipRightJoint, kneeRightJoint) + CalculateDistanceBetweenJoints(kneeRightJoint, ankleRightJoint) + CalculateDistanceBetweenJoints(ankleRightJoint, footRightJoint);
    float heightBody = heightHeadCenterToTop + heightTorso + (heightLeftLeg + heightRightLeg) / 2;

    // Return final body height
    return heightBody;
}

auto Body::GetLeftHandState() const -> const HandState
{
    return m_leftHandState;
}

auto Body::GetRightHandState() const -> const HandState
{
    return m_rightHandState;
}

auto Body::GetLeftHandConfidence() const -> const TrackingConfidence
{
    return m_leftHandConfidence;
}

auto Body::GetRightHandCondidence() const -> const TrackingConfidence
{
    return m_rightHandConfidence;
}

auto Body::GetJointMap() const -> const std::map<JointType, BodyJoint>&
{
	return m_jointMap;
}

auto Body::GetBodyFace() const -> std::shared_ptr<BodyFace>
{
    return m_face;
}

auto Body::GetBodyHDFace() const -> std::shared_ptr<BodyHDFace>
{
	return m_hdFace;
}

auto Body::SetJointMap(const std::map<JointType, BodyJoint>& jointMap) -> void
{
	m_jointMap = jointMap;
}

auto Body::SetLeftHandState(HandState lhs) -> void
{
	m_leftHandState = lhs;
}

auto Body::SetRightHandState(HandState rhs) -> void
{
	m_rightHandState = rhs;
}

auto Body::SetLeftHandConfidence(TrackingConfidence lhc) -> void
{
	m_leftHandConfidence = lhc;
}

auto Body::SetRightHandConfidence(TrackingConfidence rhc) -> void
{
	m_rightHandConfidence = rhc;
}

auto Body::SetBodyFace(std::shared_ptr<BodyFace> face) -> void
{
	m_face = face;
}

auto Body::SetBodyHDFace(std::shared_ptr<BodyHDFace> hdFace) -> void
{
	m_hdFace = hdFace;
}

auto Body::CalculateDistanceBetweenJoints(BodyJoint joint1, BodyJoint joint2) const -> const float
{
    return sqrt(pow(joint1.GetWorldPosition().X - joint2.GetWorldPosition().X, 2) + pow(joint1.GetWorldPosition().Y - joint2.GetWorldPosition().Y, 2) + pow(joint1.GetWorldPosition().Z - joint2.GetWorldPosition().Z, 2));
}

// *****************************************************************
// ************************ Joint Methods **************************
// *****************************************************************

Body::BodyJoint::BodyJoint(CameraSpacePoint positionCameraSpace, const Vector4 orientation, TrackingState trackingState)
    : m_positionCameraSpace(positionCameraSpace),
      m_orientation(orientation),
      m_trackingState(trackingState)
{
}

Body::BodyJoint::~BodyJoint()
{
}

auto Body::BodyJoint::GetWorldPosition() const -> const CameraSpacePoint&
{
    return m_positionCameraSpace;
}

auto Body::BodyJoint::GetOrientation() const -> const Vector4&
{
    return m_orientation;
}

auto Body::BodyJoint::GetTrackingState() const -> TrackingState
{
    return m_trackingState;
}

// *****************************************************************
// ************************ Face Methods ***************************
// *****************************************************************

auto Body::BodyFace::GetBoundingBoxInColorSpace() const -> const RectI
{
    return m_boundingBoxInColorSpace;
}

auto Body::BodyFace::GetPositionLeftEyeInColorSpace() const -> const cv::Point
{
    return m_positionLeftEyeInColorSpace;
}

auto Body::BodyFace::GetPositionRightEyeInColorSpace() const -> const cv::Point
{
    return m_positionRightEyeInColorSpace;
}

auto Body::BodyFace::GetPositionNoseEyeInColorSpace() const -> const cv::Point
{
    return m_positionNoseInColorSpace;
}

auto Body::BodyFace::GetPositionMouthLeftInColorSpace() const -> const cv::Point
{
    return m_positionMouthLeftInColorSpace;
}

auto Body::BodyFace::GetPositionMouthRightInColorSpace() const -> const cv::Point
{
    return m_positionMouthRightInColorSpace;
}

auto Body::BodyFace::GetFaceRotation() const -> const Vector4
{
    return m_faceRotation;
}

auto Body::BodyFace::GetFaceYawInDegrees() const -> const int
{
    return m_yawInDegrees;
}

auto Body::BodyFace::GetFacePitchInDegrees() const -> const int
{
    return m_pitchInDegrees;
}

auto Body::BodyFace::GetFaceRollInDegrees() const -> const int
{
    return m_rollInDegrees;
}

auto Body::BodyFace::GetDetectionResultHappy() const -> const DetectionResult
{
    return m_happy;
}

auto Body::BodyFace::GetDetectionResultRightEyeClosed() const -> const DetectionResult
{
    return m_rightEyeClosed;
}

auto Body::BodyFace::GetDetectionResultLeftEyeClosed() const -> const DetectionResult
{
    return m_leftEyeClosed;
}

auto Body::BodyFace::GetDetectionResultMouthOpen() const -> const DetectionResult
{
    return m_mouthOpen;
}

auto Body::BodyFace::GetDetectionResultMouthMoved() const -> const DetectionResult
{
    return m_mouthMoved;
}

auto Body::BodyFace::GetDetectionResultLookingAway() const -> const DetectionResult
{
    return m_lookingAway;
}

auto Body::BodyFace::GetDetectionResultWearingGlasses() const -> const DetectionResult
{
    return m_wearingGlasses;
}

auto Body::BodyFace::GetDetectionResultFaceEngangement() const -> const DetectionResult
{
    return m_faceEngagement;
}

auto Body::BodyFace::SetBoundingBoxInColorSpace(RectI boundingBox) -> void
{
    m_boundingBoxInColorSpace = boundingBox;
}

auto Body::BodyFace::SetPositionLeftEyeInColorSpace(cv::Point posLeftEye) -> void
{
    m_positionLeftEyeInColorSpace = posLeftEye;
}

auto Body::BodyFace::SetPositionRightEyeInColorSpace(cv::Point posRightEye) -> void
{
    m_positionRightEyeInColorSpace = posRightEye;
}

auto Body::BodyFace::SetPositionNoseEyeInColorSpace(cv::Point posNose) -> void
{
    m_positionNoseInColorSpace = posNose;
}

auto Body::BodyFace::SetPositionMouthLeftInColorSpace(cv::Point posMouthLeft) -> void
{
    m_positionMouthLeftInColorSpace = posMouthLeft;
}

auto Body::BodyFace::SetPositionMouthRightInColorSpace(cv::Point posMouthRight) -> void
{
    m_positionMouthRightInColorSpace = posMouthRight;
}

auto Body::BodyFace::SetFaceRotation(Vector4 orientation) -> void
{
    m_faceRotation = orientation;

    // Quote from Kinect for Windows SDK v2.0 Developer Preview - Samples/Native/FaceBasics-D2D, and Partial Modification
    // ExtractFaceRotationInDegrees is: Copyright (c) Microsoft Corporation. All rights reserved.
    double x = orientation.x;
    double y = orientation.y;
    double z = orientation.z;
    double w = orientation.w;

    // convert face rotation quaternion to Euler angles in degrees
    m_pitchInDegrees = static_cast<int>(std::atan2(2 * (y * z + w * x), w * w - x * x - y * y + z * z) / 3.14159265358979323846264338328 * 180.0f);
    m_yawInDegrees = static_cast<int>(std::asin(2 * (w * y - x * z)) / 3.14159265358979323846264338328 * 180.0f);
    m_rollInDegrees = static_cast<int>(std::atan2(2 * (x * y + w * z), w * w + x * x - y * y - z * z) / 3.14159265358979323846264338328 * 180.0f);
}

auto Body::BodyFace::SetDetectionResultHappy(DetectionResult res) -> void
{
    m_happy = res;
}

auto Body::BodyFace::SetDetectionResultRightEyeClosed(DetectionResult res) -> void
{
    m_rightEyeClosed = res;
}

auto Body::BodyFace::SetDetectionResultLeftEyeClosed(DetectionResult res) -> void
{
    m_leftEyeClosed = res;
}

auto Body::BodyFace::SetDetectionResultMouthOpen(DetectionResult res) -> void
{
    m_mouthOpen = res;
}

auto Body::BodyFace::SetDetectionResultMouthMoved(DetectionResult res) -> void
{
    m_mouthMoved = res;
}

auto Body::BodyFace::SetDetectionResultLookingAway(DetectionResult res) -> void
{
    m_lookingAway = res;
}

auto Body::BodyFace::SetDetectionResultWearingGlasses(DetectionResult res) -> void
{
    m_wearingGlasses = res;
}

auto Body::BodyFace::SetDetectionResultFaceEngangement(DetectionResult res) -> void
{
    m_faceEngagement = res;
}


// *****************************************************************
// ********************** HD Face Methods **************************
// *****************************************************************

auto Body::BodyHDFace::GetBoundingBoxInColorSpace() const -> const RectI
{
	return m_boundingBoxInColorSpace;
}

auto Body::BodyHDFace::GetFaceRotation() const -> const Vector4
{
	return m_faceRotation;
}

auto Body::BodyHDFace::GetFaceYawInDegrees() const -> const int
{
	return m_yawInDegrees;
}

auto Body::BodyHDFace::GetFacePitchInDegrees() const -> const int
{
	return m_pitchInDegrees;
}

auto Body::BodyHDFace::GetFaceRollInDegrees() const -> const int
{
	return m_rollInDegrees;
}

auto Body::BodyHDFace::GetHeadPivotWorldPosition() const -> const CameraSpacePoint
{
	return m_positionHeadPivotCameraSpace;
}

auto Body::BodyHDFace::GetFaceAlignmentQuality() const -> const FaceAlignmentQuality
{
	return m_alignmentQuality;
}

auto Body::BodyHDFace::GetVerticesWorldSpace() const -> const std::vector<CameraSpacePoint>&
{
	return m_verticesWorldSpace;
}

auto Body::BodyHDFace::GetPointsMinMaxXWorldSpace() const -> const std::pair<CameraSpacePoint, CameraSpacePoint>
{
	auto minMaxX = std::minmax_element(std::begin(m_verticesWorldSpace), std::end(m_verticesWorldSpace),
		[](CameraSpacePoint const& lhs, CameraSpacePoint const& rhs)
	{
		return lhs.X < rhs.X;
	});
	return std::make_pair(CameraSpacePoint{ minMaxX.first->X, minMaxX.first->Y, minMaxX.first->Z }, CameraSpacePoint{ minMaxX.second->X, minMaxX.second->Y, minMaxX.second->Z });
}

auto Body::BodyHDFace::GetPointsMinMaxYWorldSpace() const -> const std::pair<CameraSpacePoint, CameraSpacePoint>
{
	auto minMaxY = std::minmax_element(std::begin(m_verticesWorldSpace), std::end(m_verticesWorldSpace),
		[](CameraSpacePoint const& lhs, CameraSpacePoint const& rhs)
	{
		return lhs.Y < rhs.Y;
	});
	return std::make_pair(CameraSpacePoint{ minMaxY.first->X, minMaxY.first->Y, minMaxY.first->Z }, CameraSpacePoint{ minMaxY.second->X, minMaxY.second->Y, minMaxY.second->Z });
}

auto Body::BodyHDFace::GetPointsMinMaxZWorldSpace() const -> const std::pair<CameraSpacePoint, CameraSpacePoint>
{
	auto minMaxZ = std::minmax_element(std::begin(m_verticesWorldSpace), std::end(m_verticesWorldSpace),
		[](CameraSpacePoint const& lhs, CameraSpacePoint const& rhs)
		{
			return lhs.Z < rhs.Z; 
		});
	return std::make_pair(CameraSpacePoint{ minMaxZ.first->X, minMaxZ.first->Y, minMaxZ.first->Z }, CameraSpacePoint{ minMaxZ.second->X, minMaxZ.second->Y, minMaxZ.second->Z });
}

auto Body::BodyHDFace::GetCentroidWorldSpace() const -> const CameraSpacePoint
{
	float mean_x{ 0.0f };
	float mean_y{ 0.0f };
	float mean_z{ 0.0f };

	for (const auto& currentVertex : m_verticesWorldSpace)
	{
		mean_x = mean_x + currentVertex.X;
		mean_y = mean_y + currentVertex.Y;
		mean_z = mean_z + currentVertex.Z;
	}

	mean_x = mean_x / m_verticesWorldSpace.size();
	mean_y = mean_y / m_verticesWorldSpace.size();
	mean_z = mean_z / m_verticesWorldSpace.size();

	return CameraSpacePoint{ mean_x, mean_y, mean_z };
}

auto Body::BodyHDFace::SetBoundingBoxInColorSpace(RectI boundingBox) -> void
{
	m_boundingBoxInColorSpace = boundingBox;
}

auto Body::BodyHDFace::SetFaceRotation(Vector4 orientation) -> void
{
	m_faceRotation = orientation;

	// Quote from Kinect for Windows SDK v2.0 Developer Preview - Samples/Native/FaceBasics-D2D, and Partial Modification
	// ExtractFaceRotationInDegrees is: Copyright (c) Microsoft Corporation. All rights reserved.
	double x = orientation.x;
	double y = orientation.y;
	double z = orientation.z;
	double w = orientation.w;

	// convert face rotation quaternion to Euler angles in degrees
	m_pitchInDegrees = static_cast<int>(std::atan2(2 * (y * z + w * x), w * w - x * x - y * y + z * z) / 3.14159265358979323846264338328 * 180.0f);
	m_yawInDegrees = static_cast<int>(std::asin(2 * (w * y - x * z)) / 3.14159265358979323846264338328 * 180.0f);
	m_rollInDegrees = static_cast<int>(std::atan2(2 * (x * y + w * z), w * w + x * x - y * y - z * z) / 3.14159265358979323846264338328 * 180.0f);
}

auto Body::BodyHDFace::SetHeadPivotWorldPosition(CameraSpacePoint positionHeadPivotWorldSpace) -> void
{
	m_positionHeadPivotCameraSpace = positionHeadPivotWorldSpace;
}

auto Body::BodyHDFace::SetFaceAlignmentQuality(FaceAlignmentQuality faceAlignmentQuality) -> void
{
	m_alignmentQuality = faceAlignmentQuality;
}

auto Body::BodyHDFace::SetVerticesWorldSpace(const std::vector<CameraSpacePoint>& verticesWorldSpace) -> void
{
	m_verticesWorldSpace = verticesWorldSpace;
}

auto Body::BodyHDFace::GetMouthOpeningInCentimeters() -> float
{
	CameraSpacePoint p1 = m_verticesWorldSpace.at(1072); // mouthUpperlipMidbottomWorldSpace
	CameraSpacePoint p2 = m_verticesWorldSpace.at(10);   // mouthLowerlipMidtopWorldSpace

	return sqrt(pow(p1.X - p2.X, 2) + pow(p1.Y - p2.Y, 2) + pow(p1.Z - p2.Z, 2)) * 100.0f; // in cm
}
