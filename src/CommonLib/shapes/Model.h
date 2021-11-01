#pragma once

/* **********************************************************************************
#  																					#
# Copyright (c) 2015-2016,															#
# Chair for Computer Aided Medical Procedures & Augmented Reality (CAMPAR) I-16		#
# Technische Universit�t M�nchen													#
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

class Model
{
public:
	/*Loads the 3D Model.
	@param
	filename path to the model file.
	*/
	explicit Model(std::string filename);

	/*Loads the 3D Model. Use this function only, if the mesh contains only one material with one texture assigned to it.
	@param
	filename path to the model file.
	texturename path to the texture which replaces the default one assigned by the model itself
	*/
	explicit Model(std::string filename, std::string texturename);

	/*Retrieves the mesh names saved inside the model file.
	@return vector containing all mesh names.
	*/
	auto getMeshNames()->std::vector<std::string>&;

	/*Retrieves the meshes inside the model file*/
	auto getMeshes()->std::vector<std::shared_ptr<Mesh>>&;

	/*Retrieves the mesh at a specific position. Ordering is the same as in any 3D Modeling program
	Throws an index violation exception, if index is not in range of the mesh ID's*/
	auto getMesh(int id)->std::shared_ptr<Mesh>&;
    
private:
	std::string directory;
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Texture>> textures_loaded;
	std::vector<std::string> mesh_names;

	auto ProcessNode(aiNode* node, const aiScene* scene) -> void;
	auto ProcessMesh(aiMesh* mesh, const aiScene* scene) const -> std::shared_ptr<Mesh>;

	auto ProcessNode(aiNode *node, const aiScene* scene, std::string texturename) -> void;
	auto ProcessMesh(aiMesh* mesh, const aiScene* scene, std::string texturename) const->std::shared_ptr<Mesh>;
};
