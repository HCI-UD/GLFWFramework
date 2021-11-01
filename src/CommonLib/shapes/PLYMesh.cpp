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

PLYMesh::PLYMesh(std::string filename)
{
	// TODO: REWRITE TO USE C++ API functions instead of old style deprecated C98
	// Open mesh file
	//std::ifstream meshFile;
	//meshFile.open(filename);
	//meshFile.close();

	// Open mesh file
	FILE* in;
	in = fopen(filename.c_str(), "r");

	// Initialize order array
	for (int i = 0; i < 11; ++i)
	{
		order[i] = -1;
	}

	// Check header of mesh file
	if (!CheckHeader(in))
	{
		fprintf(stderr, "Error: could not read PLY file.\n");
		return;
	}

	// Read vertices
	ReadVertices(in);

	// Calculate centroid
	CalculateCentroid();

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
	if (this->hasNormals)
	{
		this->vboNormalsHandle = GLUtils::BufferUtils::CreateBufferObject(GL_ARRAY_BUFFER, &this->vec_normals[0], this->vec_normals.size() * sizeof(glm::vec3), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, this->vboNormalsHandle);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}
	// Color
	if (this->hasColors)
	{
		this->vboColorsHandle = GLUtils::BufferUtils::CreateBufferObject(GL_ARRAY_BUFFER, &this->vec_colors[0], this->vec_colors.size() * sizeof(glm::vec3), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, this->vboColorsHandle);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}
	// Texture
	if (this->hasTextureCoords)
	{
		this->vboTextCoordsHandle = GLUtils::BufferUtils::CreateBufferObject(GL_ARRAY_BUFFER, &this->vec_texcoords[0], this->vec_texcoords.size() * sizeof(glm::vec2), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, this->vboTextCoordsHandle);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	}
	// Indices
	this->vboIndicesHandle = GLUtils::BufferUtils::CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, &this->vec_indices[0], vec_indices.size() * sizeof(unsigned int), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndicesHandle);
	glBindVertexArray(0);
}

PLYMesh::~PLYMesh(void)
{
	glDeleteVertexArrays(1, &vaoHandle);

	GLUtils::BufferUtils::DeleteBufferObject(&this->vboPositionsHandle);
	GLUtils::BufferUtils::DeleteBufferObject(&this->vboNormalsHandle);
	GLUtils::BufferUtils::DeleteBufferObject(&this->vboColorsHandle);
	GLUtils::BufferUtils::DeleteBufferObject(&this->vboTextCoordsHandle);
	GLUtils::BufferUtils::DeleteBufferObject(&this->vboIndicesHandle);
}

auto PLYMesh::GetNumVertices() const -> GLuint
{
	return numVertices;
}

auto PLYMesh::GetNumFaces() const -> GLuint
{
	return numFaces;
}

auto PLYMesh::GetCentroid() const -> glm::vec3
{
	return centroid;
}

auto PLYMesh::CheckHeader(FILE* in) -> GLboolean
{
	char buf[128], type[128], c[32];
	int i;

	// read ply file header
	fscanf(in, "%s\n", buf);
	if (strcmp(buf, "ply") != 0)
	{
		fprintf(stderr, "Error: Input file is not of .ply type.\n");
		return false;
	}
	fgets(buf, 128, in);
	if (strncmp(buf, "format ascii", 12) != 0)
	{
		fprintf(stderr, "Error: Input file is not in ASCII format.\n");
		return false;
	}

	fgets(buf, 128, in);
	while (strncmp(buf, "comment", 7) == 0)
		fgets(buf, 128, in);

	// read number of vertices
	if (strncmp(buf, "element vertex", 14) == 0)
		sscanf(buf, "element vertex %d\n", &numVertices);
	else
	{
		fprintf(stderr, "Error: number of vertices expected.\n");
		return false;
	}

	// read vertex properties order
	i = 0;
	fgets(buf, 128, in);
	while (strncmp(buf, "property", 8) == 0)
	{
		sscanf(buf, "property %s %s\n", type, c);
		if (strncmp(c, "x", 1) == 0)
			order[0] = i;
		else if (strncmp(c, "y", 1) == 0)
			order[1] = i;
		else if (strncmp(c, "z", 1) == 0)
			order[2] = i;

		else if (strncmp(c, "nx", 2) == 0)
			order[3] = i;
		else if (strncmp(c, "ny", 2) == 0)
			order[4] = i;
		else if (strncmp(c, "nz", 2) == 0)
			order[5] = i;

		else if (strncmp(c, "red", 3) == 0)
			order[6] = i;
		else if (strncmp(c, "green", 5) == 0)
			order[7] = i;
		else if (strncmp(c, "blue", 4) == 0)
			order[8] = i;

		else if (strncmp(c, "tu", 2) == 0)
			order[9] = i;
		else if (strncmp(c, "tv", 2) == 0)
			order[10] = i;

		i++;
		fgets(buf, 128, in);
	}

	for (i = 0; i < 3; i++)
	{
		if (order[i] < 0)
		{
			fprintf(stderr, "Error: not enough vertex coordinate fields (nx, ny, nz).\n");
			return false;
		}
	}
	hasNormals = true;
	for (i = 3; i < 6; i++)
		if (order[i] < 0)
			hasNormals = false;
	hasColors = true;
	for (i = 6; i < 9; i++)
		if (order[i] < 0)
			hasColors = false;
	hasTextureCoords = true;
	for (i = 9; i < 11; i++)
		if (order[i] < 0)
			hasTextureCoords = false;

	if (!hasNormals)
		fprintf(stderr, "Warning: no normal coordinates used from file.\n");
	if (!hasColors)
		fprintf(stderr, "Warning: no color used from file.\n");
	if (!hasTextureCoords)
		fprintf(stderr, "Warning: no texture coordinates used from file.\n");

	// number of faces and face properties
	if (strncmp(buf, "element face", 12) == 0)
		sscanf(buf, "element face %d\n", &numFaces);
	else
	{
		fprintf(stderr, "Error: number of faces expected.\n");
		return false;
	}

	fgets(buf, 128, in);
	if (strncmp(buf, "property list", 13) != 0)
	{
		fprintf(stderr, "Error: property list expected.\n");
		return false;
	}

	fgets(buf, 128, in);
	while (strncmp(buf, "end_header", 10) != 0)
		fgets(buf, 128, in);

	return true;
}

auto PLYMesh::ReadVertices(FILE* in) -> void
{
	char buf[128];
	unsigned int i;
	float values[32];
	unsigned int faces[4];

	// Read in vertex attributes
	for (i = 0; i < numVertices; i++)
	{
		fgets(buf, 128, in);
		sscanf(buf, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5], &values[6], &values[7], &values[8], &values[9], &values[10], &values[11], &values[12], &values[13], &values[14], &values[15]);

		// Positions x,y,z
		float x = values[0];
		float y = values[1];
		float z = values[2];
		vec_vertices.push_back(glm::vec3(x, y, z));

		// Normals nx,ny,nz
		if (hasNormals)
		{
			float nx = values[3];
			float ny = values[4];
			float nz = values[5];
			vec_normals.push_back(glm::vec3(nx, ny, nz));
		}

		// Colors r,g,b
		if (hasColors)
		{
			float r = values[6];
			float g = values[7];
			float b = values[8];
			vec_colors.push_back(glm::vec3(r, g, b));
		}

		// Texture Coordinates s,t
		if (hasTextureCoords)
		{
			float s = values[9];
			float t = values[10];
			vec_texcoords.push_back(glm::vec2(s, t));
		}
	}

	// FACES
	for (i = 0; i < numFaces; i++)
	{
		fgets(buf, 128, in);
		sscanf(buf, "%d %d %d %d", &faces[0], &faces[1], &faces[2], &faces[3]);

		// Indices 
		vec_indices.push_back(faces[1]);
		vec_indices.push_back(faces[2]);
		vec_indices.push_back(faces[3]);
	}
}

auto PLYMesh::CalculateCentroid() -> void
{
	// Calculate the simplified centroid (mean in all directions)
	float mean_x = 0.0f;
	float mean_y = 0.0f;
	float mean_z = 0.0f;

	for (unsigned int i = 0; i < this->numVertices; ++i)
	{
		mean_x = mean_x + this->vec_vertices.at(i).x;
		mean_y = mean_y + this->vec_vertices.at(i).y;
		mean_z = mean_z + this->vec_vertices.at(i).z;
	}

	mean_x = mean_x / numVertices;
	mean_y = mean_y / numVertices;
	mean_z = mean_z / numVertices;

	this->centroid = glm::vec3(mean_x, mean_y, mean_z);
}

void PLYMesh::Render(GLenum type) const
{
	glBindVertexArray(vaoHandle);
	glDrawElements(type, static_cast<GLsizei>(vec_indices.size()), GL_UNSIGNED_INT, 0);
}
