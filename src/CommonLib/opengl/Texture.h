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

class Texture
{
public:
	Texture(GLenum target,
		    const GLvoid* data = nullptr,
		    GLsizei width = 0, 
		    GLsizei height = 0, 
		    GLsizei depth = 0,
		    GLint   internalFormat = GL_RGB, 
		    GLint   format = GL_BGR_EXT, 
			GLint   level = 0,
		    GLint   border = 0,
		    GLenum  pixelDataType = GL_UNSIGNED_BYTE,
		    GLint   magFilter = GL_LINEAR, 
		    GLint   minFilter = GL_LINEAR, 
		    GLint   unpackAlignment = 1,
		    GLenum  textureWrapS = GL_REPEAT,
		    GLenum  textureWrapT = GL_REPEAT);
	virtual ~Texture();

	auto GetHandle() const -> const GLuint;
	auto Bind()      const -> const void;
	auto Unbind()    const -> const void;
	auto UpdateTexture(const GLvoid* data, GLint xoffset, GLsizei width, GLint yoffset = 0, GLsizei height = 0, GLint zoffset = 0, GLsizei depth = 0) const -> void;

private:
	GLuint  m_handle{ 0 };

	GLenum  m_target{ GL_TEXTURE_2D };
	GLsizei m_width{ 0 };
	GLsizei m_height{ 0 };
	GLsizei m_depth{ 0 };
	GLenum  m_internalFormat{ 0 };
	GLenum  m_format{ 0 };
	GLint   m_level{ 0 };
	GLint   m_border{ 0 };
	GLenum  m_pixelDataType{ GL_UNSIGNED_BYTE };
	GLint   m_magFilter{ 0 };
	GLint   m_minFilter{ 0 };
	GLint   m_unpackAlignment{ 0 };
	GLenum  m_textureWrapS{ GL_REPEAT };
	GLenum  m_textureWrapT{ GL_REPEAT };
};
