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

FramebufferObject::FramebufferObject()
{
	glGenFramebuffers(1, &fboHandle);
}

FramebufferObject::~FramebufferObject()
{
	glDeleteFramebuffers(1, &fboHandle);

	for (auto& rboHandle : rboHandles)
	{
		glDeleteRenderbuffers(1, &rboHandle);
	}
}

auto FramebufferObject::GetHandle() const ->GLuint
{
	return fboHandle;
}

auto FramebufferObject::AttachTexture2D(GLuint textureHandle) -> void
{
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	for (auto& currentColorAttachement : colorAttachements)
	{
		if (currentColorAttachement.second == 0)
		{
			currentColorAttachement.second = textureHandle;
			glFramebufferTexture2D(GL_FRAMEBUFFER, currentColorAttachement.first, GL_TEXTURE_2D, textureHandle, 0);
			break;
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

auto FramebufferObject::AttachRenderbuffer(RenderBufferAttachement attachement, GLenum internalFormat, GLsizei width, GLsizei height) -> void
{
	GLuint rboHandle{ 0 };
	glGenRenderbuffers(1, &rboHandle);
	rboHandles.push_back(rboHandle);

	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
	glBindRenderbuffer(GL_RENDERBUFFER, rboHandle);
	glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
	switch (attachement)
	{
	case (RenderBufferAttachement::COLOR) :
	{
		for (auto& currentColorAttachement : colorAttachements)
		{
			if (currentColorAttachement.second == 0)
			{
				currentColorAttachement.second = rboHandle;
				glFramebufferTexture2D(GL_FRAMEBUFFER, currentColorAttachement.first, GL_TEXTURE_2D, rboHandle, 0);
				break;
			}
		}
		break;
	}

	case (RenderBufferAttachement::DEPTH) :
	{
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboHandle);
		break;
	}

	case (RenderBufferAttachement::STENCIL) :
	{
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboHandle);
		break;
	}

	case (RenderBufferAttachement::DEPTH_STENCIL) :
	{
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboHandle);
		break;
	}

	default:
	{
		break;
	}
	}
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

auto FramebufferObject::Validate() -> void
{
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
	std::vector<GLenum> activeColorAttachements;
	for (const auto& currentColorAttachement : colorAttachements)
	{
		if (currentColorAttachement.second != 0)
		{
			activeColorAttachements.push_back(currentColorAttachement.first);
		}
	}
	glDrawBuffers(static_cast<GLsizei>(activeColorAttachements.size()), reinterpret_cast<GLenum*>(&activeColorAttachements.at(0)));

	GLenum fboStatus{ GL_FRAMEBUFFER_COMPLETE };
	fboStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		std::string error{ "Error while trying to setup custom FBO." };
		std::cerr << error << std::endl;
		throw std::runtime_error(error.c_str());
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}