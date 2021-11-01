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

Sphere::Sphere(float radius, int rings, int sectors, glm::vec3 color, glm::vec3 center) : m_radius(radius), m_rings(rings), m_sectors(sectors), m_center(center)
{
    // Create Data
    float const R = 1.0f / static_cast<float>(rings - 1);
    float const S = 1.0f / static_cast<float>(sectors - 1);

    for (int r = 0; r < rings; ++r)
    {
        for (int s = 0; s < sectors; ++s)
        {
            float phi   = 2.0f * 3.14159265358979323846264338328f * s * S;
            float theta = 3.14159265358979323846264338328f * r * R;

            float const x = cos(phi) * sin(theta);
            float const y = cos(theta);
            float const z = sin(phi) * sin(theta);

            glm::vec3 v = glm::vec3(x * radius, y * radius, z * radius);
            glm::vec3 n = glm::vec3(x, y, z);
            glm::vec3 c = color;
            glm::vec2 t = glm::vec2(s * S, r * R);

            vec_vertices.push_back(v);
            vec_normals.push_back(n);
            vec_colors.push_back(c);
            vec_texcoords.push_back(t);
        }
    }

    for (int r = 0; r < rings - 1; ++r)
    {
        for (int s = 0; s < sectors - 1; ++s)
        {
            vec_indices.push_back(r * sectors + s);
            vec_indices.push_back(r * sectors + (s + 1));
            vec_indices.push_back((r + 1) * sectors + (s + 1));

            vec_indices.push_back(r * sectors + s);
            vec_indices.push_back((r + 1) * sectors + (s + 1));
            vec_indices.push_back((r + 1) * sectors + s);
        }
    }

    // Fill VAO
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);
        // Positions
        this->vboPositionsHandle = GLUtils::BufferUtils::CreateBufferObject(GL_ARRAY_BUFFER, &this->vec_vertices[0], this->vec_vertices.size() * sizeof(glm::vec3), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboPositionsHandle);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Normals
        this->vboNormalsHandle = GLUtils::BufferUtils::CreateBufferObject(GL_ARRAY_BUFFER, &this->vec_normals[0], this->vec_normals.size() * sizeof(glm::vec3), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboNormalsHandle);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Colors
        this->vboColorsHandle = GLUtils::BufferUtils::CreateBufferObject(GL_ARRAY_BUFFER, &this->vec_colors[0], this->vec_colors.size() * sizeof(glm::vec3), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboColorsHandle);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //Texture Coords
        this->vboTextCoordsHandle = GLUtils::BufferUtils::CreateBufferObject(GL_ARRAY_BUFFER, &this->vec_texcoords[0], this->vec_texcoords.size() * sizeof(glm::vec2), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboTextCoordsHandle);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Indices
        this->vboIndicesHandle = GLUtils::BufferUtils::CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, &this->vec_indices[0], vec_indices.size() * sizeof(unsigned int), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndicesHandle);
    glBindVertexArray(0);
}

Sphere::~Sphere()
{
    glDeleteVertexArrays(1, &vaoHandle);

    GLUtils::BufferUtils::DeleteBufferObject(&this->vboPositionsHandle);
    GLUtils::BufferUtils::DeleteBufferObject(&this->vboNormalsHandle);
    GLUtils::BufferUtils::DeleteBufferObject(&this->vboColorsHandle);
    GLUtils::BufferUtils::DeleteBufferObject(&this->vboTextCoordsHandle);
    GLUtils::BufferUtils::DeleteBufferObject(&this->vboIndicesHandle);
}

auto Sphere::GetRingCount() const -> GLuint
{
    return m_rings;
}

auto Sphere::GetSectorCount() const -> GLuint
{
    return m_sectors;
}

auto Sphere::GetRadius() const -> float
{
    return m_radius;
}

auto Sphere::GetCenter() const -> glm::vec3
{
    return m_center;
}

auto Sphere::Render(GLenum type) const -> void
{
    glBindVertexArray(vaoHandle);

    switch (type)
    {
        case(GL_TRIANGLES) :
        {
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->vec_indices.size()), GL_UNSIGNED_INT, nullptr);
            break;
        }

        case(GL_POINTS) :
        {
            glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(this->vec_vertices.size()));
            break;
        }

        case(GL_LINES) :
        {
            glDrawElements(GL_LINES, static_cast<GLsizei>(this->vec_indices.size()), GL_UNSIGNED_INT, nullptr);
            break;
        }

        case(GL_LINE_STRIP) :
        {
            glDrawElements(GL_LINE_STRIP, static_cast<GLsizei>(this->vec_indices.size()), GL_UNSIGNED_INT, nullptr);
            break;
        }

        case(GL_LINE_LOOP) :
        {
            glDrawArrays(GL_LINE_LOOP, 0, static_cast<GLsizei>(this->vec_vertices.size()));
            break;
        }
    }

    glBindVertexArray(0);
}