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

class Camera
{
public:
    // Getter
    auto GetPosition()    const -> const glm::vec3&;
    auto GetOrientation() const -> glm::mat4;
    auto GetFieldOfView() const -> float;
    auto GetNearPlane()   const -> float;
    auto GetFarPlane()    const -> float;
    auto GetAspectRatio() const -> float;

    // Setter
    auto SetPosition(const glm::vec3& position) -> void;
    auto SetFieldOfView(float fieldOfView)      -> void;
    auto SetNearPlane(float nearPlane)          -> void;
    auto SetFarPlane(float farPlane)            -> void;
    auto SetAspectRatio(float aspectRatio)      -> void;

    // Direction unit vector getter
    auto Forward() const -> glm::vec3;
    auto Right()   const -> glm::vec3;
    auto Up()      const -> glm::vec3;

    // Matrix Getters
    auto GetViewMatrix()       const -> glm::mat4;
    auto GetProjectionMatrix() const -> glm::mat4;
    auto GetCombinedVPMatrix() const -> glm::mat4;

    // Camera control methods
    auto OffsetPosition(const glm::vec3& positionOffset) -> void;
    auto OffsetOrientation(float upAngleInDegrees, float rightAngleInDegrees) -> void;
    auto LookAt(glm::vec3 position) -> void;

    // Convenience methods
    auto ProcessScrollEvent(double scrollValue) -> void;

private:
    glm::vec3 m_position{glm::vec3(0.0f, 0.0f, 2.0f)};
    float m_horizontalAngle{0.0f};
    float m_verticalAngle{0.0f};

    float m_fieldOfView{50.0f};
    float m_nearPlane{0.01f};
    float m_farPlane{1000.0f};
    float m_aspectRatio{4.0f / 3.0f};

    float m_zoomSensitivity{2.0f};
    float m_translationSensitivity{2.5f};
    float m_rotationSensitivity{0.1f};

    auto NormalizeAngles() -> void;
};
