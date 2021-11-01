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

enum class LightingModel { DIFFUSE_ONLY, PHONG };

class SubroutineLighting : public GLFWApp
{
public:
    // GLFW callbacks
    auto virtual OnKey(int key, int scancode, int action, int mods) -> void override
    {
        if (action != GLFW_PRESS)
        {
            return;
        }

        switch (key)
        {
            case GLFW_KEY_ESCAPE:
            {
                glfwSetWindowShouldClose(m_window, 1);
                break;
            }

            case GLFW_KEY_1:
            {
                lightingModel = LightingModel::DIFFUSE_ONLY;
                std::cout << "Lighting Model: DIFFUSE_ONLY" << std::endl;
                break;
            }

            case GLFW_KEY_2:
            {
                lightingModel = LightingModel::PHONG;
                std::cout << "Lighting Model: PHONG" << std::endl;
                break;
            }
        }
    }

    auto virtual OnMouseMove(double x, double y) -> void override
    {
        glm::ivec2 mousePosition = glm::ivec2(x, y);
        glm::vec2 mouseDelta = glm::vec2(mousePosition - m_lastMousePosition);
        m_lastMousePosition = mousePosition;

        if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            camera->OffsetOrientation(mouseDelta.y, mouseDelta.x);
        }

        if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            if (mouseDelta.y > 0)
            {
                camera->OffsetPosition(m_timeBetweenLastUpdateInSeconds * -camera->Forward());
            }
            else
            {
                camera->OffsetPosition(m_timeBetweenLastUpdateInSeconds * camera->Forward());
            }
        }
    }

    auto virtual OnScroll(double x, double y) -> void override
    {
        camera->ProcessScrollEvent(-y);
    }

    auto virtual OnMouseButton(int button, int action, int mods) -> void override
    {
        if (action == GLFW_PRESS)
        {
            double x, y;
            glfwGetCursorPos(m_window, &x, &y);
            m_lastMousePosition = glm::ivec2(x, y);
            m_lastMouseButtonPressed = button;
        }
    }

    // GLFW window loop functions
    auto virtual InitGL() -> void override
    {
        // OpenGL state
        glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
        glEnable(GL_DEPTH_TEST);

        // UBO Matrices (binding index 0)
        ubo_matrices = GLUtils::BufferUtils::CreateBufferObject(GL_UNIFORM_BUFFER, nullptr, 2 * sizeof(glm::mat4), GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_matrices);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // UBO Light (binding index 1)
        glm::vec4 La = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec4 Ld = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec4 Ls = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec4 Le = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec4 LightPosition = glm::vec4(0.0f, 100.0f, 0.0f, 1.0f);
        ubo_lightInfo = GLUtils::BufferUtils::CreateBufferObject(GL_UNIFORM_BUFFER, nullptr, 5 * sizeof(glm::vec4), GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_lightInfo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_lightInfo);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), glm::value_ptr(La));
            glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(Ld));
            glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(Ls));
            glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(Le));
            glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(LightPosition));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // UBO Material (binding index 2)
        glm::vec4 Ka = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
        glm::vec4 Kd = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
        glm::vec4 Ks = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
        glm::vec4 Ke = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
        float Shininess = 50.0f;
        ubo_materialInfo = GLUtils::BufferUtils::CreateBufferObject(GL_UNIFORM_BUFFER, nullptr, 4 * sizeof(glm::vec4) + 1 * sizeof(float), GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubo_materialInfo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_materialInfo);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), glm::value_ptr(Ka));
            glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(Kd));
            glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(Ks));
            glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(Ke));
            glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4), sizeof(float), &Shininess);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Pointers
        skull = std::make_shared<PLYMesh>("../../resources/meshes/skull.ply");
        camera = std::make_shared<Camera>();

        // Programs
        prg_lighting = std::make_shared<ShaderProgram>("../../resources/shaders/Lighting.vs", "../../resources/shaders/Lighting.fs");
    }

    auto virtual Update() -> void override
    {
        // Process keyboard input for camera movement
        double elapsedTimeInSeconds = glfwGetTime();
        m_timeBetweenLastUpdateInSeconds = static_cast<float>(elapsedTimeInSeconds - m_timeSinceLastUpdateInSeconds);
        m_timeSinceLastUpdateInSeconds = elapsedTimeInSeconds;

        if (glfwGetKey(m_window, 'S'))
        {
            camera->OffsetPosition(m_timeBetweenLastUpdateInSeconds * -camera->Forward());
        }
        else if (glfwGetKey(m_window, 'W'))
        {
            camera->OffsetPosition(m_timeBetweenLastUpdateInSeconds * camera->Forward());
        }
        if (glfwGetKey(m_window, 'A'))
        {
            camera->OffsetPosition(m_timeBetweenLastUpdateInSeconds * -camera->Right());
        }
        else if (glfwGetKey(m_window, 'D'))
        {
            camera->OffsetPosition(m_timeBetweenLastUpdateInSeconds * camera->Right());
        }
        if (glfwGetKey(m_window, 'Z'))
        {
            camera->OffsetPosition(m_timeBetweenLastUpdateInSeconds * -glm::vec3(0, 1, 0));
        }
        else if (glfwGetKey(m_window, 'X'))
        {
            camera->OffsetPosition(m_timeBetweenLastUpdateInSeconds * glm::vec3(0, 1, 0));
        }
    }

    auto virtual Draw() -> void override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update camera UBO
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->GetViewMatrix()));
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera->GetProjectionMatrix()));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Render skull
        prg_lighting->UseProgram();
            switch (lightingModel)
            {
                case(LightingModel::DIFFUSE_ONLY):
                {
                    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ssi_diffuseIndex);
                    break;
                }

                case(LightingModel::PHONG):
                {
                    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ssi_phongIndex);
                    break;
                }
            }
            skull->ResetModelMatrix();
			skull->ScaleModel(glm::vec3(0.005f, 0.005f, 0.005f));
            skull->RotateModel(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            prg_lighting->SetUniform("modelMatrix", skull->GetModelMatrix());
            skull->Render(GL_TRIANGLES);
        prg_lighting->UnUseProgram();
    }

    auto virtual ShutdownGL() -> void override
    {
        GLUtils::BufferUtils::DeleteBufferObject(&ubo_matrices);
        GLUtils::BufferUtils::DeleteBufferObject(&ubo_lightInfo);
        GLUtils::BufferUtils::DeleteBufferObject(&ubo_materialInfo);
    }

private:
    glm::ivec2 m_lastMousePosition{glm::ivec2()};
    int m_lastMouseButtonPressed{0};
    double m_timeSinceLastUpdateInSeconds{0.0};
    float m_timeBetweenLastUpdateInSeconds{0.0f};

    GLuint ubo_matrices{0};
    GLuint ubo_lightInfo{0};
    GLuint ubo_materialInfo{0};

    GLuint ssi_phongIndex{0};
    GLuint ssi_diffuseIndex{1};

    std::shared_ptr<PLYMesh> skull{nullptr};
    std::shared_ptr<Camera> camera{nullptr};

    std::shared_ptr<ShaderProgram> prg_lighting{nullptr};

    LightingModel lightingModel{LightingModel::DIFFUSE_ONLY};
};

int main(int argc, char ** argv)
{
    try
    {
        return SubroutineLighting().Run();
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