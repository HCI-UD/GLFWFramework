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

Model::Model(std::string filename)
{
	mesh_names = std::vector<std::string>();
	Assimp::Importer importer;
	auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
	
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::string error{ importer.GetErrorString() };
		std::cerr << error << std::endl;
		throw std::runtime_error(error.c_str());
	}

	this->directory = filename.substr(0, filename.find_last_of('/'));

	this->ProcessNode(scene->mRootNode, scene);
}

Model::Model(std::string filename, std::string texturename)
{
	mesh_names = std::vector<std::string>();
	Assimp::Importer importer;
	auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::string error{ importer.GetErrorString() };
		std::cerr << error << std::endl;
		throw std::runtime_error(error.c_str());
	}

	this->directory = filename.substr(0, filename.find_last_of('/'));

	this->ProcessNode(scene->mRootNode, scene, texturename);
}

auto Model::ProcessNode(aiNode* node, const aiScene* scene) -> void
{
	// Process every mesh located at the current node
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		std::shared_ptr<Mesh> processedMesh = ProcessMesh(mesh, scene);
		this->meshes.push_back(processedMesh);
	}

	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene);
		mesh_names.push_back(node->mChildren[i]->mName.C_Str());
	}
}

auto Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) const -> std::shared_ptr<Mesh>
{
	std::vector<glm::vec3> vec_positions;
	std::vector<glm::vec3> vec_normals;
	std::vector<glm::vec2> vec_texcoords;
	std::vector<GLuint> vec_indices;
	std::vector<std::shared_ptr<Texture>> vec_textures;

	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		// Position
		glm::vec3 position; 
		position.x = mesh->mVertices[i].x;
		position.y = mesh->mVertices[i].y;
		position.z = mesh->mVertices[i].z;
		vec_positions.push_back(position);

		// Normal
		glm::vec3 normal;
		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;
		vec_normals.push_back(normal);

		// Texture Coordinate (only if present)
		if (mesh->mTextureCoords[0]) 
		{
			glm::vec2 texcoord;
			texcoord.x = mesh->mTextureCoords[0][i].x;
			texcoord.y = mesh->mTextureCoords[0][i].y;
			vec_texcoords.push_back(texcoord);
		}
		else
		{
			vec_texcoords.push_back(glm::vec2(0.0f, 0.0f));
		}
	}

	// Retrieve the corresponding vertex indices for every face of the mesh.
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			vec_indices.push_back(face.mIndices[j]);
		}
	}

	// Process materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		for (GLuint i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
		{
			aiString str;
			material->GetTexture(aiTextureType_DIFFUSE, i, &str);

			std::string filename = std::string(str.C_Str());
			filename = this->directory + '/' + filename;
			cv::Mat imageMat = cv::imread(filename.c_str(), CV_LOAD_IMAGE_COLOR);
			std::shared_ptr<Texture> tex = std::make_shared<Texture>(GL_TEXTURE_2D, imageMat.data, imageMat.cols, imageMat.rows);
			vec_textures.push_back(tex);
		}
	}

	// Return a mesh object created from the extracted mesh data
	std::shared_ptr<Mesh> processedMesh = std::make_shared<Mesh>(vec_positions, vec_normals, vec_texcoords, vec_indices, vec_textures);
	return processedMesh;
}

auto Model::ProcessNode(aiNode * node, const aiScene * scene, std::string texturename) -> void
{
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		std::shared_ptr<Mesh> processedMesh = ProcessMesh(mesh, scene, texturename);
		this->meshes.push_back(processedMesh);
	}

	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene, texturename);
		mesh_names.push_back(node->mChildren[i]->mName.C_Str());
	}
}

auto Model::ProcessMesh(aiMesh * mesh, const aiScene * scene, std::string filename) const -> std::shared_ptr<Mesh>
{
	std::vector<glm::vec3> vec_positions;
	std::vector<glm::vec3> vec_normals;
	std::vector<glm::vec2> vec_texcoords;
	std::vector<GLuint> vec_indices;
	std::vector<std::shared_ptr<Texture>> vec_textures;

	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		// Position
		glm::vec3 position;
		position.x = mesh->mVertices[i].x;
		position.y = mesh->mVertices[i].y;
		position.z = mesh->mVertices[i].z;
		vec_positions.push_back(position);

		// Normal
		glm::vec3 normal;
		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;
		vec_normals.push_back(normal);

		// Texture Coordinate (only if present)
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 texcoord;
			texcoord.x = mesh->mTextureCoords[0][i].x;
			texcoord.y = mesh->mTextureCoords[0][i].y;
			vec_texcoords.push_back(texcoord);
		}
		else
		{
			vec_texcoords.push_back(glm::vec2(0.0f, 0.0f));
		}
	}

	// Retrieve the corresponding vertex indices for every face of the mesh.
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			vec_indices.push_back(face.mIndices[j]);
		}
	}

	// Process materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		if( material->GetTextureCount(aiTextureType_DIFFUSE) == 1)
		{
			filename = this->directory + '/' + filename;
			cv::Mat imageMat = cv::imread(filename.c_str(), CV_LOAD_IMAGE_COLOR);
			std::shared_ptr<Texture> tex = std::make_shared<Texture>(GL_TEXTURE_2D, imageMat.data, imageMat.cols, imageMat.rows);
			vec_textures.push_back(tex);
		}
	}

	// Return a mesh object created from the extracted mesh data
	std::shared_ptr<Mesh> processedMesh = std::make_shared<Mesh>(vec_positions, vec_normals, vec_texcoords, vec_indices, vec_textures);
	return processedMesh;
}

auto Model::getMeshNames() -> std::vector<std::string>&
{
	return mesh_names;
}

auto Model::getMeshes() -> std::vector<std::shared_ptr<Mesh>>&
{
	return meshes;
}

auto Model::getMesh(int id) -> std::shared_ptr<Mesh>&
{
	if (id < meshes.size() && id >= 0) {
		return meshes[id];
	}
	else {
		std::cout << "Invalid index in mesh retrievement occured" << std::endl;
		return meshes[-1];	//provoke error msg
	}
}
