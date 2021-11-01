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

/*
* Creates an OpenGL buffer object
*
* Official OpenGL Documentation:
* glGenBuffers: https://www.opengl.org/sdk/docs/man/html/glGenBuffers.xhtml
* glBindBuffer: http://www.opengl.org/sdk/docs/man/html/glBindBuffer.xhtml
* glBufferData: https://www.opengl.org/sdk/docs/man/html/glBufferData.xhtml
*/
auto GLUtils::BufferUtils::CreateBufferObject(GLenum type, const GLvoid *data, size_t sizeOfData, GLenum usageHint) -> GLuint
{
    // 1. Declare buffer object ID
    GLuint bufferObjectID;

    // 2. Generate one buffer objects name and store it in the handle bufferObjectID
    glGenBuffers(1, &bufferObjectID);

    // 3. Bind the buffer object to the given target, depending on the type
    glBindBuffer(type, bufferObjectID);

    // 4. Create and initialize the buffer object data store, that is currently bound to the given target
    glBufferData(type, sizeOfData, data, usageHint);

    // 5. After the data store is initialized, unbind the buffer object by binding 0 to the given target
    glBindBuffer(type, 0);

    // Return buffer object ID
    return bufferObjectID;
}

/*
* Deletes an OpenGL buffer object
*
* Official OpenGL Documentation:
* glDeleteBuffers: https://www.opengl.org/sdk/docs/man/html/glDeleteBuffers.xhtml
*/
auto GLUtils::BufferUtils::DeleteBufferObject(GLuint *bufferObject) -> void
{
    // 1. Deletes one buffer object and automatically unbinds it from its previously bount target
    glDeleteBuffers(1, bufferObject);

    // 2. Set buffer object to 0
    *bufferObject = 0;
}
