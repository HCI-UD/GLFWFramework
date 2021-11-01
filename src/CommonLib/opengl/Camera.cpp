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

// Getter
auto Camera::GetPosition() const -> const glm::vec3&
{
    return m_position;
}

auto Camera::GetOrientation() const -> glm::mat4
{
    glm::mat4 orientation;
    orientation = glm::rotate(orientation, glm::radians(m_verticalAngle), glm::vec3(1, 0, 0));
    orientation = glm::rotate(orientation, glm::radians(m_horizontalAngle), glm::vec3(0, 1, 0));
    return orientation;
}

auto Camera::GetFieldOfView() const -> float
{
    return m_fieldOfView;
}

auto Camera::GetNearPlane() const -> float
{
    return m_nearPlane;
}

auto Camera::GetFarPlane() const -> float
{
    return m_farPlane;
}

auto Camera::GetAspectRatio() const -> float
{
    return m_aspectRatio;
}

// Setter
auto Camera::SetPosition(const glm::vec3& position) -> void
{
    m_position = position;
}

auto Camera::SetFieldOfView(float fieldOfView) -> void
{
    if (fieldOfView > 0.0f && fieldOfView < 180.0f)
    {
        m_fieldOfView = fieldOfView;
    }
    else
    {
        std::string error{"FoV value has to be between 0 and 180 degrees."};
        std::cerr << error << std::endl;
        throw std::runtime_error(error.c_str());
    }
}

auto Camera::SetNearPlane(float nearPlane) -> void
{
    if (nearPlane > 0.0f)
    {
        m_nearPlane = nearPlane;
    }
    else
    {
        std::string error{"Near clipping plane value has to be greater than 0."};
        std::cerr << error << std::endl;
        throw std::runtime_error(error.c_str());
    }
}

auto Camera::SetFarPlane(float farPlane) -> void
{
    if (farPlane > m_nearPlane)
    {
        m_farPlane = farPlane;
    }
    else
    {
        std::string error{"Far plane has to be greater than near plane."};
        std::cerr << error << std::endl;
        throw std::runtime_error(error.c_str());
    }
}

auto Camera::SetAspectRatio(float aspectRatio) -> void
{
    if (aspectRatio > 0.0f)
    {
        m_aspectRatio = aspectRatio;
    }
    else
    {
        std::string error{"Aspect ratio value has to be greater than 0."};
        std::cerr << error << std::endl;
        throw std::runtime_error(error.c_str());
    }
}

// Direction unit vector getter
auto Camera::Forward() const -> glm::vec3
{
    glm::vec4 forward = glm::inverse(GetOrientation()) * glm::vec4(0, 0, -1, 1);
    return glm::vec3(forward);
}

auto Camera::Right() const -> glm::vec3
{
    glm::vec4 right = glm::inverse(GetOrientation()) * glm::vec4(1, 0, 0, 1);
    return glm::vec3(right);
}

auto Camera::Up() const -> glm::vec3
{
    glm::vec4 up = glm::inverse(GetOrientation()) * glm::vec4(0, 1, 0, 1);
    return glm::vec3(up);
}

// Matrix Getters
auto Camera::GetViewMatrix() const -> glm::mat4
{
    return GetOrientation() * glm::translate(glm::mat4(), -m_position);
}

auto Camera::GetProjectionMatrix() const -> glm::mat4
{
    return glm::perspective(glm::radians(m_fieldOfView), m_aspectRatio, m_nearPlane, m_farPlane);
}

auto Camera::GetCombinedVPMatrix() const -> glm::mat4
{
    return GetProjectionMatrix() * GetViewMatrix();
}

// Camera control methods
auto Camera::OffsetPosition(const glm::vec3& positionOffset) -> void
{
    m_position += positionOffset * m_translationSensitivity;
}

auto Camera::OffsetOrientation(float upAngleInDegrees, float rightAngleInDegrees) -> void
{
    m_horizontalAngle += m_rotationSensitivity * rightAngleInDegrees;
    m_verticalAngle += m_rotationSensitivity * upAngleInDegrees;
    NormalizeAngles();
}

auto Camera::LookAt(glm::vec3 position) -> void
{
    if (position != m_position)
    {
        glm::vec3 direction = glm::normalize(position - m_position);
        m_verticalAngle = glm::radians(asinf(-direction.y));
        m_horizontalAngle = -glm::radians(atan2f(-direction.x, -direction.z));
        NormalizeAngles();
    }
    else
    {
        std::string error{"LookAt position identical to current camera position."};
        std::cerr << error << std::endl;
        throw std::runtime_error(error.c_str());
    }
}

// Convenience methods
auto Camera::ProcessScrollEvent(double scrollValue) -> void
{
    float fieldOfView = m_fieldOfView + m_zoomSensitivity * static_cast<float>(scrollValue);

    if (fieldOfView < 5.0f)
    {
        fieldOfView = 5.0f;
    }

    if (fieldOfView > 130.0f)
    {
        fieldOfView = 130.0f;
    }

    m_fieldOfView = fieldOfView;
}

auto Camera::NormalizeAngles() -> void
{
    m_horizontalAngle = fmodf(m_horizontalAngle, 360.0f);
    
    // Make sure the horizintal angle is positive as fmod can return negative values
    if (m_horizontalAngle < 0.0f)
    {
        m_horizontalAngle += 360.0f;
    }

    // Make sure the vertical angle is less than 90 degrees to avoid gimbal lock
    if (m_verticalAngle > 85.0f)
    {
        m_verticalAngle = 85.0f;
    }
    else if (m_verticalAngle < -85.0f)
    {
        m_verticalAngle = -85.0f;
    }
}
