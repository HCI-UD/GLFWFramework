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

class DeviceOptions
{
public:
    DeviceOptions();
    ~DeviceOptions();

    auto GetDeviceUID()                  const -> std::string;
    auto IsAudioEnabled()                const -> bool;
    auto IsColorEnabled()                const -> bool;
    auto IsDepthEnabled()                const -> bool;
    auto IsInfraredEnabled()             const -> bool;
    auto IsLongExposureInfraredEnabled() const -> bool;
    auto IsBodyIndexEnabled()            const -> bool;
    auto IsBodyEnabled()                 const -> bool;
    auto IsFaceEnabled()                 const -> bool;
	auto IsHDFaceEnabled()               const -> bool;
    auto SetDeviceUID(const std::string& uid = "0")                           -> DeviceOptions&;
    auto SetIsAudioEnabled(bool isAudioEnabled)                               -> DeviceOptions&;
    auto SetIsColorEnabled(bool isColorEnabled)                               -> DeviceOptions&;
    auto SetIsDepthEnabled(bool isDepthEnabled)                               -> DeviceOptions&;
    auto SetIsInfraredEnabled(bool isInfraredEnabled)                         -> DeviceOptions&;
    auto SetIsLongExposureInfraredEnabled(bool isLongExposureInfraredEnabled) -> DeviceOptions&;
    auto SetIsBodyIndexEnabled(bool isBodyIndexEnabled)                       -> DeviceOptions&;
    auto SetIsBodyEnabled(bool isBodyEnabled)                                 -> DeviceOptions&;
    auto SetIsFaceEnabled(bool isFaceEnabled)                                 -> DeviceOptions&;
	auto SetIsHDFaceEnabled(bool isHDFaceEnabled)                             -> DeviceOptions&;

private:
    std::string m_deviceUID = "0";
    bool m_isAudioEnabled{false};
    bool m_isColorEnabled{true};
    bool m_isDepthEnabled{true};
    bool m_isInfraredEnabled{false};
    bool m_isLongExposureInfraredEnabled{false};
    bool m_isBodyIndexEnabled{false};
    bool m_isBodyEnabled{false};
    bool m_isFaceEnabled{false};
	bool m_isHDFaceEnabled{false};
};

