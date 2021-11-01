#pragma once

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

class Mesh : public Drawable
{
public:   
	/*Create an instance of Mesh*/
	Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> texCoords, std::vector<GLuint> indices, std::vector<std::shared_ptr<Texture>> textures);
	virtual ~Mesh();

	/*Render the mesh*/
	auto Render(GLenum type) const -> void override;
	
	/*Retrieves 3D coordinate of the midpoint of the AABB of the mesh*/
	auto getCentroid() const->glm::vec3;

	/*Retrieves side length in x, y, z axis of the AABB*/
	auto getSideLength() const->glm::vec3;

	/*Retrieves the AABB, consistent of the minimum (x, y, z) and maximum (x, y, z) coordinate*/
	auto getAABB() const -> std::vector<glm::vec3>;

	/*Retrieves costumized AABB which can be altered by the function resizeAABB*/
	auto getCostumAABB() const->std::vector<glm::vec3>;

	/*Rescale or apply an offset to the original AABB
	@param
	offset(X|Y|Z) offset for 3 axes, will be added independantly from the scaling and AABB size
	(x|y|z)Scale relational scaling, dependant from side length of the AABB, e.g. xScale := 0.5f resizes the side length in x-axis by 50%. Midpoint is preserved*/
	auto resizeAABB(float offsetX, float offsetY, float offsetZ, float xScale, float yScale, float zScale) -> void;

private:
	GLuint vaoHandle{ 0 };

	GLuint vboPositionsHandle{ 0 };
	GLuint vboNormalsHandle{ 0 };
	GLuint vboTextCoordsHandle{ 0 };
	GLuint vboIndicesHandle{ 0 };

	std::vector<glm::vec3> vec_positions;
	std::vector<glm::vec3> vec_normals;
	std::vector<glm::vec2> vec_texcoords;
	std::vector<unsigned int> vec_indices;
	std::vector<std::shared_ptr<Texture>> vec_textures;

	glm::vec3 m_centroid;
	glm::vec3 m_sideLength;
	std::vector<glm::vec3> vec_aabb;
	std::vector<glm::vec3> m_costumaabb;
};