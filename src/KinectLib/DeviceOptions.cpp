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

DeviceOptions::DeviceOptions()
{
}

DeviceOptions::~DeviceOptions()
{
}

auto DeviceOptions::GetDeviceUID() const -> std::string
{
    return m_deviceUID;
}

auto DeviceOptions::IsAudioEnabled() const -> bool
{
    return m_isAudioEnabled;
}

auto DeviceOptions::IsColorEnabled() const -> bool
{
    return m_isColorEnabled;
}

auto DeviceOptions::IsDepthEnabled() const -> bool
{
    return m_isDepthEnabled;
}

auto DeviceOptions::IsInfraredEnabled() const -> bool
{
    return m_isInfraredEnabled;
}

auto DeviceOptions::IsLongExposureInfraredEnabled() const -> bool
{
    return m_isLongExposureInfraredEnabled;
}

auto DeviceOptions::IsBodyIndexEnabled() const -> bool
{
    return m_isBodyIndexEnabled;
}

auto DeviceOptions::IsBodyEnabled() const -> bool
{
    return m_isBodyEnabled;
}

auto DeviceOptions::IsFaceEnabled() const -> bool
{
    return m_isFaceEnabled;
}

auto DeviceOptions::IsHDFaceEnabled() const -> bool
{
	return m_isHDFaceEnabled;
}

DeviceOptions& DeviceOptions::SetDeviceUID(const std::string& uid)
{
    m_deviceUID = uid;
    return *this;
}

DeviceOptions& DeviceOptions::SetIsAudioEnabled(bool isAudioEnabled)
{
    m_isAudioEnabled = isAudioEnabled;
    return *this;
}

DeviceOptions& DeviceOptions::SetIsColorEnabled(bool isColorEnabled)
{
    m_isColorEnabled = isColorEnabled;
    return *this;
}

DeviceOptions& DeviceOptions::SetIsDepthEnabled(bool isDepthEnabled)
{
    m_isDepthEnabled = isDepthEnabled;
    return *this;
}

DeviceOptions& DeviceOptions::SetIsInfraredEnabled(bool isInfraredEnabled)
{
    m_isInfraredEnabled = isInfraredEnabled;
    return *this;
}

DeviceOptions& DeviceOptions::SetIsLongExposureInfraredEnabled(bool isLongExposureInfraredEnabled)
{
    m_isLongExposureInfraredEnabled = isLongExposureInfraredEnabled;
    return *this;
}

DeviceOptions& DeviceOptions::SetIsBodyIndexEnabled(bool isBodyIndexEnabled)
{
    m_isBodyIndexEnabled = isBodyIndexEnabled;
    return *this;
}

DeviceOptions& DeviceOptions::SetIsBodyEnabled(bool isBodyEnabled)
{
    m_isBodyEnabled = isBodyEnabled;
    return *this;
}

auto DeviceOptions::SetIsFaceEnabled(bool isFaceEnabled) -> DeviceOptions&
{
    m_isFaceEnabled = isFaceEnabled;
    return *this;
}

auto DeviceOptions::SetIsHDFaceEnabled(bool isHDFaceEnabled) -> DeviceOptions&
{
	m_isHDFaceEnabled = isHDFaceEnabled;
	return *this;
}
