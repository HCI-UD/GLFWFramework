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

class Drawable
{
public:
    virtual ~Drawable();

    auto GetIsRendered()        const -> bool;
    auto GetTranslationMatrix() const -> glm::mat4;
    auto GetRotationMatrix()    const -> glm::mat4;
    auto GetScaleMatrix()       const -> glm::mat4;
    auto GetPosition()          const -> glm::vec3;
    auto GetModelMatrix()       const -> glm::mat4;

    auto SetIsRendered(bool isRendered)     -> void;
    auto SetTranslationMatrix(glm::mat4 &T) -> void;
    auto SetRotationMatrix(glm::mat4 &R)    -> void;
    auto SetScaleMatrix(glm::mat4 &S)       -> void;

    auto TranslateModel(glm::vec3 translationVector)           -> void;
    auto RotateModel(float angleInRadiants, glm::vec3 rotAxis) -> void;
    auto ScaleModel(glm::vec3 scalingVector)                   -> void;

    auto ResetModelMatrix() -> void;

    virtual auto Render(GLenum type) const -> void = 0;

protected:
    bool m_isRendered{true};
	glm::mat4 m_translationMatrix{glm::mat4()};
    glm::mat4 m_rotationMatrix{glm::mat4()};
    glm::mat4 m_scaleMatrix{glm::mat4()};
};