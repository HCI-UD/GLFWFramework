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

class ColoredTriangle : public GLFWApp
{
public:
    // GLFW window loop functions
    auto virtual InitGL() -> void override
    {
        // OpenGL state
        glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

        // Triangle
        std::vector<float> triangleVertexData =
        {
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
             0.0f,  1.0f, 0.0f,
        };

        std::vector<float> triangleColorData = 
        {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
        };

        glGenVertexArrays(1, &vao_triangle);
        glBindVertexArray(vao_triangle);
            // Positions
            vbo_trianglePositions = GLUtils::BufferUtils::CreateBufferObject(GL_ARRAY_BUFFER, &triangleVertexData[0], triangleVertexData.size() * sizeof(float), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_trianglePositions);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Colors
            vbo_triangleColors = GLUtils::BufferUtils::CreateBufferObject(GL_ARRAY_BUFFER, &triangleColorData[0], triangleColorData.size() * sizeof(float), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_triangleColors);
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // UBO Matrices (binding index 0)
        ubo_matrices = GLUtils::BufferUtils::CreateBufferObject(GL_UNIFORM_BUFFER, nullptr, 2 * sizeof(glm::mat4), GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_matrices);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Programs
        prg_triangle = std::make_shared<ShaderProgram>("../../resources/shaders/SIMPLE.vs", "../../resources/shaders/SIMPLE.fs");
    }

    auto virtual Draw() -> void override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        prg_triangle->UseProgram();
            prg_triangle->SetUniform("modelMatrix", glm::scale(glm::mat4(), glm::vec3(0.5f, 0.5f, 0.5f)));
            glBindVertexArray(vao_triangle);
                glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        prg_triangle->UnUseProgram();
    }

    auto virtual ShutdownGL() -> void override
    {
        glDeleteVertexArrays(1, &vao_triangle);

        GLUtils::BufferUtils::DeleteBufferObject(&vbo_trianglePositions);
        GLUtils::BufferUtils::DeleteBufferObject(&vbo_triangleColors);
        GLUtils::BufferUtils::DeleteBufferObject(&ubo_matrices);
    }

private:
    GLuint vao_triangle{0};
    GLuint vbo_trianglePositions{0};
    GLuint vbo_triangleColors{0};
    GLuint ubo_matrices{0};

    std::shared_ptr<ShaderProgram> prg_triangle{nullptr};
};

int main(int argc, char ** argv)
{
    try
    {
        return ColoredTriangle().Run();
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