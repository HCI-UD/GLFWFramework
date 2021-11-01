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

BackgroundQuad::BackgroundQuad()
{

    // Image Layout 
    //
    // The image quad is split into two 
    // triangles: {0,1,2} and {2,3,0} !!
    //
    // See: http://stackoverflow.com/questions/16809833/opencv-image-loading-for-opengl-texture
    //
    //               ^
    //   {3}         |         {2}
    // (-1,1)--------|--------(1,1)
    //    |          |          |
    //    |          |          |
    //    |          |          |
    //  ---------- (0,0) ---------->
    //    |          |          |
    //    |          |          |
    //    |          |          |
    // (-1,-1)-------|--------(1,-1)
    //   {0}         |          {1}
    //
    std::vector<float> vec_positions =
    {
        // First triangle
        -1.0f, -1.0f,  0.0f, // {0}
         1.0f, -1.0f,  0.0f, // {1}
         1.0f,  1.0f,  0.0f, // {2}

         // Second triangle
         1.0f,  1.0f,  0.0f, // {2}
        -1.0f,  1.0f,  0.0f, // {3}
        -1.0f, -1.0f,  0.0f  // {0}
    };

    // Texel Layout 
    //
    // In the regular GL texel layout,
    // the bottom left corner is (0,0).
    // Due to the fact that OpenCV stores
    // images from top to bottom, unlike
    // GL which uses bottom to top, the 
    // image read with OpenCV has to be
    // flipped vertically OR (better and
    // faster), the texel coordinates 
    // have to be flipped vertically, 
    // i.e. the origin is in top left
    // instead of top bottom.
    //
    // Regular OpenGL texel layout: 
    // 
    //   ^
    //   |
    //   |
    //  {3}                  {2}
    // (0,1)----------------(1,1)
    //   |                    |
    //   |                    |
    //   |                    |
    //   |                    |
    //   |                    |
    //   |                    |
    //   |                    |
    // (0,0)----------------(1,0)--->
    //  {0}                  {1}
    //
    // 
    // Used texel layout according to OpenCV: 
    // 
    //  {0}                  {1}
    // (0,0)----------------(1,0)--->
    //   |                    |
    //   |                    |
    //   |                    |
    //   |                    |
    //   |                    |
    //   |                    |
    //   |                    |
    // (0,1)----------------(1,1)
    //  {3}                  {2}
    //   |
    //   |
    //   ^
    //
    std::vector<float> vec_texCoords =
    {
        // First triangle
        0.0f, 1.0f, // {3}
        1.0f, 1.0f, // {2]
        1.0f, 0.0f, // {1}

        // Second triangle
        1.0f, 0.0f, // {1}
        0.0f, 0.0f, // {0}
        0.0f, 1.0f  // {3}
    };

    // Create VAO with quad positions and texture coords
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
        // Positions
        m_vboPositions = GLUtils::BufferUtils::CreateBufferObject(GL_ARRAY_BUFFER, &vec_positions[0], vec_positions.size() * sizeof(float), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Texture coords
        m_vboTexCoords = GLUtils::BufferUtils::CreateBufferObject(GL_ARRAY_BUFFER, &vec_texCoords[0], vec_texCoords.size() * sizeof(float), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboTexCoords);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

BackgroundQuad::~BackgroundQuad()
{
    glDeleteVertexArrays(1, &m_vao);
    GLUtils::BufferUtils::DeleteBufferObject(&m_vboPositions);
    GLUtils::BufferUtils::DeleteBufferObject(&m_vboTexCoords);
}

auto BackgroundQuad::Render() const -> void
{
	glBindVertexArray(m_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
