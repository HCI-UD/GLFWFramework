#include "Common.h"
#include <ft2build.h>
#include FT_FREETYPE_H

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

class BasicGLFWwindow : public GLFWApp
{
public:
    // GLFW window loop functions
    auto virtual InitGL() -> void override
    {
		// OpenGL state
		glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Freetype library and font loading
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			std::string error{ "Unable to initialize freetype library." };
			std::cerr << error << std::endl;
			throw std::runtime_error(error.c_str());
		}

		FT_Face face;
		if (FT_New_Face(ft, m_pathToFont.c_str(), 0, &face))
		{
			std::string error{ "Unable to load specified font." };
			std::cerr << error << std::endl;
			throw std::runtime_error(error.c_str());
		}

		FT_Set_Pixel_Sizes(face, 0, m_fontSize);

		for (GLubyte currentASCIIcharacter = 0; currentASCIIcharacter < 128; ++currentASCIIcharacter)
		{
			if (FT_Load_Char(face, currentASCIIcharacter, FT_LOAD_RENDER))
			{
				std::string error{ "Unable to load glyph for character." };
				std::cerr << error << std::endl;
				throw std::runtime_error(error.c_str());
			}

			std::shared_ptr<Texture> glyphTexture = std::make_shared<Texture>(GL_TEXTURE_2D, face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_RED, 0, 0, GL_UNSIGNED_BYTE, GL_LINEAR, GL_LINEAR, 1, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
			CharacterGlyph characterGlyph = { glyphTexture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), static_cast<GLuint>(face->glyph->advance.x) };
			m_characters.insert(std::pair<GLchar, CharacterGlyph>(currentASCIIcharacter, characterGlyph));
		}

		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		// Pointers
		m_textRenderer = std::make_shared<TextRenderer>(m_characters);

		// Programs
		prg_text = std::make_shared<ShaderProgram>("../../resources/shaders/Text.vs", "../../resources/shaders/Text.fs");
		prg_text->UseProgram();
			prg_text->SetUniform("projection", glm::ortho(0.0f, static_cast<GLfloat>(m_windowDimensions.x), static_cast<GLfloat>(m_windowDimensions.y), 0.0f));
			prg_text->SetUniform("text", 0);
			prg_text->SetUniform("textColor", glm::vec3(1.0f, 0.0f, 0.0f));
		prg_text->UnUseProgram();
    }

    auto virtual Draw() -> void override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		prg_text->UseProgram();
			glActiveTexture(GL_TEXTURE0);
				m_textRenderer->RenderText("freetype test", 50.0f, 150.0f, 1.0f);
		prg_text->UnUseProgram();
    }

private:
	std::string m_pathToFont{ "../../resources/fonts/ARDESTINE.TTF" };
	GLuint m_fontSize{ 54 };
	std::map<GLchar, CharacterGlyph> m_characters{{}};
	std::shared_ptr<TextRenderer> m_textRenderer{ nullptr };

	std::shared_ptr<ShaderProgram> prg_text{ nullptr };
};

int main(int argc, char ** argv)
{
    try
    {
        return BasicGLFWwindow().Run();
    }
    catch (std::exception& error)
    {
        std::cerr << error.what() << std::endl;
    }
    catch (const std::string& error)
    {
        std::cerr << error.c_str() << std::endl;
    }

    return -1;
}