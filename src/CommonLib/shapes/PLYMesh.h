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

class PLYMesh : public Drawable
{

public:
    PLYMesh(std::string filename);
    virtual ~PLYMesh(void);

    auto GetNumVertices() const -> GLuint;
    auto GetNumFaces()    const -> GLuint;
    auto GetCentroid()    const -> glm::vec3;

    auto Render(GLenum type) const -> void override;

private:
    GLuint    vaoHandle{0};

    GLuint    vboPositionsHandle{0};
    GLuint    vboNormalsHandle{0};
    GLuint    vboColorsHandle{0};
    GLuint    vboTextCoordsHandle{0};
    GLuint    vboIndicesHandle{0};

    GLuint    numVertices{0};
    GLuint    numFaces{0};
    GLboolean hasNormals;
    GLboolean hasColors;
    GLboolean hasTextureCoords;
    glm::vec3 centroid{glm::vec3()};

    std::vector<glm::vec3> vec_vertices;
    std::vector<glm::vec3> vec_normals;
    std::vector<glm::vec3> vec_colors;
    std::vector<glm::vec2> vec_texcoords;
    std::vector<unsigned int> vec_indices;

	int order[11]; // x,y,z,nx,ny,nz,red,green,blue,tu,tv

	// Private member functions
    auto CheckHeader(FILE *in)  ->GLboolean;
    auto ReadVertices(FILE *in) -> void;
    auto CalculateCentroid()    -> void;
};