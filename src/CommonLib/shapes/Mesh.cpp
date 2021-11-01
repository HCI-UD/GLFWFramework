#include "Common.h"
#include <limits>
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

Mesh::Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> texCoords, std::vector<GLuint> indices, std::vector<std::shared_ptr<Texture>> textures)
{
	this->vec_positions = positions;
	this->vec_normals   = normals;
	this->vec_texcoords = texCoords;
	this->vec_indices   = indices;
	this->vec_textures  = textures;

	// Fill VAO
	glGenVertexArrays(1, &this->vaoHandle);
	glBindVertexArray(this->vaoHandle);
		// Positions
		this->vboPositionsHandle = GLUtils::BufferUtils::CreateBufferObject(GL_ARRAY_BUFFER, &this->vec_positions[0], this->vec_positions.size() * sizeof(glm::vec3), GL_STATIC_DRAW);
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

		// Texture Coords
		this->vboTextCoordsHandle = GLUtils::BufferUtils::CreateBufferObject(GL_ARRAY_BUFFER, &this->vec_texcoords[0], this->vec_texcoords.size() * sizeof(glm::vec2), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, this->vboTextCoordsHandle);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Indices
		this->vboIndicesHandle = GLUtils::BufferUtils::CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, &this->vec_indices[0], vec_indices.size() * sizeof(unsigned int), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndicesHandle);
	glBindVertexArray(0);

	//computes the AABB by retrieving min\max values in each axis
	float minX = std::numeric_limits<float>::max(); float maxX = -std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max(); float maxY = -std::numeric_limits<float>::max();
	float minZ = std::numeric_limits<float>::max(); float maxZ = -std::numeric_limits<float>::max();
	for (int i = 0; i < vec_positions.size(); i++) {
		glm::vec3 v = vec_positions.at(i);

		if (minX > v.x) {
			minX = v.x;
		}
		if (maxX < v.x) {
			maxX = v.x;
		}
		if (minY > v.y) {
			minY = v.y;
		}
		if (maxY < v.y) {
			maxY = v.y;
		}
		if (minZ > v.z) {
			minZ = v.z;
		}
		if (maxZ < v.z) {
			maxZ = v.z;
		}
		
	}
	vec_aabb = std::vector<glm::vec3>();
	vec_aabb.push_back(glm::vec3(minX, minY, minZ));
	vec_aabb.push_back(glm::vec3(maxX, maxY, maxZ));
	m_sideLength = glm::vec3(std::abs(maxX - minX), std::abs(maxY - minY), std::abs(maxZ - minZ));
	m_centroid = glm::vec3((maxX + minX) / 2, (maxY + minY) / 2, (maxZ + minZ) / 2);
	m_costumaabb = vec_aabb;
}

Mesh::~Mesh(void)
 {
	glDeleteVertexArrays(1, &this->vaoHandle);
	
	GLUtils::BufferUtils::DeleteBufferObject(&this->vboPositionsHandle);
	GLUtils::BufferUtils::DeleteBufferObject(&this->vboNormalsHandle);
	GLUtils::BufferUtils::DeleteBufferObject(&this->vboTextCoordsHandle);
	GLUtils::BufferUtils::DeleteBufferObject(&this->vboIndicesHandle);
}

auto Mesh::Render(GLenum type) const -> void
{
	for (GLuint i = 0; i < this->vec_textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		this->vec_textures[i]->Bind();
	}

	glBindVertexArray(this->vaoHandle);
		glDrawElements(GL_TRIANGLES, this->vec_indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	for (GLuint i = 0; i < this->vec_textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		this->vec_textures[i]->Bind();
	}
}

auto Mesh::getCentroid() const -> glm::vec3
{
	return m_centroid;
}

auto Mesh::getSideLength() const -> glm::vec3
{
	return m_sideLength;
}

auto Mesh::getAABB() const -> std::vector<glm::vec3>
{
	return vec_aabb;
}

auto Mesh::getCostumAABB() const -> std::vector<glm::vec3>
{
	return m_costumaabb;
}

auto Mesh::resizeAABB(float offsetX, float offsetY, float offsetZ, float xScale, float yScale, float zScale) -> void
{
	m_costumaabb = std::vector<glm::vec3>();
	m_costumaabb.push_back(glm::vec3(
		(xScale == 0 ? vec_aabb[0].x : m_centroid.x - xScale * m_sideLength.x / 2 ) + offsetX,
		(yScale == 0 ? vec_aabb[0].y : m_centroid.y - yScale * m_sideLength.y / 2) + offsetY,
		(zScale == 0 ? vec_aabb[0].z : m_centroid.z - zScale * m_sideLength.z / 2) + offsetZ));
	m_costumaabb.push_back(glm::vec3(
		(xScale == 0 ? vec_aabb[1].x : m_centroid.x + xScale * m_sideLength.x / 2) + offsetX,
		(yScale == 0 ? vec_aabb[1].y : m_centroid.y + yScale * m_sideLength.y / 2) + offsetY,
		(zScale == 0 ? vec_aabb[1].z : m_centroid.z + zScale * m_sideLength.z / 2) + offsetZ));
}
