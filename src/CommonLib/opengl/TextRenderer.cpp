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

TextRenderer::TextRenderer(std::map<GLchar, CharacterGlyph> characters) :
	m_characters(characters)
{
    glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
		m_vbo = GLUtils::BufferUtils::CreateBufferObject(GL_ARRAY_BUFFER, nullptr, sizeof(GLfloat) * 6 * 4, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

TextRenderer::~TextRenderer()
{
	glDeleteVertexArrays(1, &m_vao);
	GLUtils::BufferUtils::DeleteBufferObject(&m_vbo);
}

void TextRenderer::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale)
{
    glBindVertexArray(m_vao);
		// Iterate through all characters
		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			CharacterGlyph ch = m_characters[*c];

			GLfloat xpos = x + ch.bearing.x * scale;
			GLfloat ypos = y + (m_characters['H'].bearing.y - ch.bearing.y) * scale;

			GLfloat w = ch.glyphSize.x * scale;
			GLfloat h = ch.glyphSize.y * scale;

			// Update VBO for each character
			GLfloat vertices[6][4] = {
				{ xpos,     ypos + h,   0.0, 1.0 },
				{ xpos + w, ypos,       1.0, 0.0 },
				{ xpos,     ypos,       0.0, 0.0 },

				{ xpos,     ypos + h,   0.0, 1.0 },
				{ xpos + w, ypos + h,   1.0, 1.0 },
				{ xpos + w, ypos,       1.0, 0.0 }
			};

			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// Render glyph texture over quad
			ch.glyphTextureID->Bind();
				glDrawArrays(GL_TRIANGLES, 0, 6);
			ch.glyphTextureID->Unbind();

			x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
		}
    glBindVertexArray(0);
}