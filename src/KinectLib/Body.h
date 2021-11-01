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

class Body
{
public:

    class BodyJoint
    {
    public:
        BodyJoint(CameraSpacePoint positionCameraSpace, const Vector4 orientation, TrackingState trackingState);
        ~BodyJoint();

        auto GetWorldPosition()  const -> const CameraSpacePoint&;
        auto GetOrientation()    const -> const Vector4&;
        auto GetTrackingState()  const ->       TrackingState;

    private:
        CameraSpacePoint m_positionCameraSpace = CameraSpacePoint{0.0f, 0.0f, 0.0f};
        Vector4 m_orientation = Vector4{0.0f, 0.0f, 0.0f, 0.0f};
        TrackingState m_trackingState{TrackingState_NotTracked};
    };

    class BodyFace
    {
    public:
        auto GetBoundingBoxInColorSpace()          const -> const RectI;
        auto GetPositionLeftEyeInColorSpace()      const -> const cv::Point;
        auto GetPositionRightEyeInColorSpace()     const -> const cv::Point;
        auto GetPositionNoseEyeInColorSpace()      const -> const cv::Point;
        auto GetPositionMouthLeftInColorSpace()    const -> const cv::Point;
        auto GetPositionMouthRightInColorSpace()   const -> const cv::Point;
        auto GetFaceRotation()                     const -> const Vector4;
        auto GetFaceYawInDegrees()                 const -> const int;
        auto GetFacePitchInDegrees()               const -> const int;
        auto GetFaceRollInDegrees()                const -> const int;
        auto GetDetectionResultHappy()             const -> const DetectionResult;
        auto GetDetectionResultRightEyeClosed()    const -> const DetectionResult;
        auto GetDetectionResultLeftEyeClosed()     const -> const DetectionResult;
        auto GetDetectionResultMouthOpen()         const -> const DetectionResult;
        auto GetDetectionResultMouthMoved()        const -> const DetectionResult;
        auto GetDetectionResultLookingAway()       const -> const DetectionResult;
        auto GetDetectionResultWearingGlasses()    const -> const DetectionResult;
        auto GetDetectionResultFaceEngangement()   const -> const DetectionResult;

        auto SetBoundingBoxInColorSpace(RectI)                  -> void;
        auto SetPositionLeftEyeInColorSpace(cv::Point)          -> void;
        auto SetPositionRightEyeInColorSpace(cv::Point)         -> void;
        auto SetPositionNoseEyeInColorSpace(cv::Point)          -> void;
        auto SetPositionMouthLeftInColorSpace(cv::Point)        -> void;
        auto SetPositionMouthRightInColorSpace(cv::Point)       -> void;
        auto SetFaceRotation(Vector4)                           -> void;
        auto SetDetectionResultHappy(DetectionResult)           -> void;
        auto SetDetectionResultRightEyeClosed(DetectionResult)  -> void;
        auto SetDetectionResultLeftEyeClosed(DetectionResult)   -> void;
        auto SetDetectionResultMouthOpen(DetectionResult)       -> void;
        auto SetDetectionResultMouthMoved(DetectionResult)      -> void;
        auto SetDetectionResultLookingAway(DetectionResult)     -> void;
        auto SetDetectionResultWearingGlasses(DetectionResult)  -> void;
        auto SetDetectionResultFaceEngangement(DetectionResult) -> void;

    private:
        RectI      m_boundingBoxInColorSpace = RectI{0, 0, 1, 1};
        cv::Point  m_positionLeftEyeInColorSpace = cv::Point(0, 0);
        cv::Point  m_positionRightEyeInColorSpace = cv::Point(0, 0);
        cv::Point  m_positionNoseInColorSpace = cv::Point(0, 0);
        cv::Point  m_positionMouthLeftInColorSpace = cv::Point(0, 0);
        cv::Point  m_positionMouthRightInColorSpace = cv::Point(0, 0);
        Vector4    m_faceRotation = Vector4{0.0f, 0.0f, 0.0f, 0.0f};
        int        m_yawInDegrees{0};
        int        m_pitchInDegrees{0};
        int        m_rollInDegrees{0};
        DetectionResult m_happy{DetectionResult_Unknown};
        DetectionResult m_rightEyeClosed{DetectionResult_Unknown};
        DetectionResult m_leftEyeClosed{DetectionResult_Unknown};
        DetectionResult m_mouthOpen{DetectionResult_Unknown};
        DetectionResult m_mouthMoved{DetectionResult_Unknown};
        DetectionResult m_lookingAway{DetectionResult_Unknown};
        DetectionResult m_wearingGlasses{DetectionResult_Unknown};
        DetectionResult m_faceEngagement{DetectionResult_Unknown};
    };

	class BodyHDFace
	{
	public:
		auto GetBoundingBoxInColorSpace()    const -> const RectI;
		auto GetFaceRotation()               const -> const Vector4;
		auto GetFaceYawInDegrees()           const -> const int;
		auto GetFacePitchInDegrees()         const -> const int;
		auto GetFaceRollInDegrees()          const -> const int;
		auto GetHeadPivotWorldPosition()     const -> const CameraSpacePoint;
		auto GetFaceAlignmentQuality()       const -> const FaceAlignmentQuality;
		auto GetVerticesWorldSpace()         const -> const std::vector<CameraSpacePoint>&;
		auto GetPointsMinMaxXWorldSpace()    const -> const std::pair<CameraSpacePoint, CameraSpacePoint>;
		auto GetPointsMinMaxYWorldSpace()    const -> const std::pair<CameraSpacePoint, CameraSpacePoint>;
		auto GetPointsMinMaxZWorldSpace()    const -> const std::pair<CameraSpacePoint, CameraSpacePoint>;
		auto GetCentroidWorldSpace()         const -> const CameraSpacePoint;

		auto SetBoundingBoxInColorSpace(RectI)                           -> void;
		auto SetFaceRotation(Vector4)                                    -> void;
		auto SetHeadPivotWorldPosition(CameraSpacePoint)                 -> void;
		auto SetFaceAlignmentQuality(FaceAlignmentQuality)               -> void;
		auto SetVerticesWorldSpace(const std::vector<CameraSpacePoint>&) -> void;

		auto GetMouthOpeningInCentimeters() -> float;

	private:
		RectI                         m_boundingBoxInColorSpace = RectI{ 0, 0, 1, 1 };
		Vector4                       m_faceRotation = Vector4{ 0.0f, 0.0f, 0.0f, 0.0f };
		int                           m_yawInDegrees{ 0 };
		int                           m_pitchInDegrees{ 0 };
		int                           m_rollInDegrees{ 0 };
		CameraSpacePoint              m_positionHeadPivotCameraSpace = CameraSpacePoint{ 0.0f, 0.0f, 0.0f };
		FaceAlignmentQuality          m_alignmentQuality{ FaceAlignmentQuality::FaceAlignmentQuality_Low };
		std::vector<CameraSpacePoint> m_verticesWorldSpace{};
	};

    Body(uint64_t bodyId, const std::map<JointType, BodyJoint>& jointMap, HandState leftHandState, HandState rightHandState, TrackingConfidence leftHandConfidence, TrackingConfidence rightHandConfidence, std::shared_ptr<BodyFace> const& face, std::shared_ptr<BodyHDFace> const& hdFace);
    ~Body();

    auto GetID()                  const -> const uint64_t;
    auto GetBodyHeight()          const -> const float;
    auto GetJointMap()            const -> const std::map<JointType, BodyJoint>&;
    auto GetLeftHandState()       const -> const HandState;
    auto GetRightHandState()      const -> const HandState;
    auto GetLeftHandConfidence()  const -> const TrackingConfidence;
    auto GetRightHandCondidence() const -> const TrackingConfidence;
    auto GetBodyFace()            const ->       std::shared_ptr<BodyFace>;
	auto GetBodyHDFace()          const ->       std::shared_ptr<BodyHDFace>;

	auto SetJointMap(const std::map<JointType, BodyJoint>&) -> void;
	auto SetLeftHandState(HandState)                        -> void;
	auto SetRightHandState(HandState)                       -> void;
	auto SetLeftHandConfidence(TrackingConfidence)          -> void;
	auto SetRightHandConfidence(TrackingConfidence)         -> void;
	auto SetBodyFace(std::shared_ptr<BodyFace>)             -> void;
	auto SetBodyHDFace(std::shared_ptr<BodyHDFace>)         -> void;

    auto CalculateDistanceBetweenJoints(BodyJoint joint1, BodyJoint joint2) const -> const float;

private:
    uint64_t           m_bodyID{0};
    HandState          m_leftHandState{HandState_Unknown};
    HandState          m_rightHandState{HandState_Unknown};
    TrackingConfidence m_leftHandConfidence{TrackingConfidence_Low};
    TrackingConfidence m_rightHandConfidence{TrackingConfidence_Low};

	std::map<JointType, BodyJoint> m_jointMap{ {} };
	std::shared_ptr<BodyFace>      m_face{ nullptr };
	std::shared_ptr<BodyHDFace>    m_hdFace{ nullptr };
};

