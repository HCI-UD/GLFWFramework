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

Texture::Texture(GLenum target, const GLvoid* data, GLsizei width, GLsizei height, GLsizei depth, GLint internalFormat, GLint format, GLint level, GLint border, GLenum pixelDataType, GLint magFilter, GLint minFilter, GLint unpackAlignment, GLenum textureWrapS, GLenum textureWrapT) :
	m_target(target),
	m_width(width),
	m_height(height),
	m_depth(depth),
	m_internalFormat(internalFormat),
	m_format(format),
	m_level(level),
	m_border(border),
	m_pixelDataType(pixelDataType),
	m_magFilter(magFilter),
	m_minFilter(minFilter),
	m_unpackAlignment(unpackAlignment),
	m_textureWrapS(textureWrapS),
	m_textureWrapT(textureWrapT)
{
	glGenTextures(1, &m_handle);

	this->Bind();
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, m_magFilter);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, m_minFilter);
		glTexParameteri(target, GL_TEXTURE_WRAP_S, m_textureWrapS);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, m_textureWrapT);
		glPixelStorei(GL_UNPACK_ALIGNMENT, m_unpackAlignment);
		switch(target)
		{
			case GL_TEXTURE_1D:
			{
				glTexImage1D(m_target, m_level, m_internalFormat, m_width, m_border, m_format, m_pixelDataType, data);
				break;
			}

			case GL_TEXTURE_2D:
			{
				glTexImage2D(m_target, m_level, m_internalFormat, m_width, m_height, m_border, m_format, m_pixelDataType, data);
				break;
			}

			case GL_TEXTURE_3D:
			{
				glTexImage3D(m_target, m_level, m_internalFormat, m_width, m_height, m_depth, m_border, m_format, m_pixelDataType, data);
				break;
			}
		}
	this->Unbind();
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_handle);
}

auto Texture::Bind() const -> const void
{
	glBindTexture(m_target, m_handle);
}

auto Texture::Unbind() const -> const void
{
	glBindTexture(m_target, 0);
}

auto Texture::GetHandle() const -> const GLuint
{
	return m_handle;
}

auto Texture::UpdateTexture(const GLvoid* data, GLint xoffset, GLsizei width, GLint yoffset, GLsizei height, GLint zoffset, GLsizei depth) const -> void
{
	this->Bind();
		switch (m_target)
		{
			case GL_TEXTURE_1D:
			{
				glTexSubImage1D(m_target, m_level, xoffset, width, m_format, m_pixelDataType, data);
				break;
			}

			case GL_TEXTURE_2D:
			{
				glTexSubImage2D(m_target, m_level, xoffset, yoffset, width, height, m_format, m_pixelDataType, data);
				break;
			}

			case GL_TEXTURE_3D:
			{
				glTexSubImage3D(m_target, m_level, xoffset, yoffset, zoffset, width, height, depth, m_format, m_pixelDataType, data);
				break;
			}
		}
	this->Unbind();
}