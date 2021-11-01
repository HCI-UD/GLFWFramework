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

/* 
 * BLOOM Effect example. The results are not that impressive, as no HDR colors are used. Effect would be stronger for HDR.
 */

class SubroutineBloomEffect : public GLFWApp
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

            case GLFW_KEY_SPACE:
            {
				bloomEnabled = !bloomEnabled;
                break;
            }

            case GLFW_KEY_K:
            {
                gaussianKernelSize = gaussianKernelSize - 2;
                if (gaussianKernelSize < 3)
                {
                    gaussianKernelSize = 3;
                }
                prg_postProcessing->UseProgram();
                    prg_postProcessing->SetUniform("GaussianKernelSize", gaussianKernelSize);
                prg_postProcessing->UnUseProgram();
                std::cout << "Gaussian Kernel Size: " << gaussianKernelSize << std::endl;
                break;
            }

            case GLFW_KEY_L:
            {
                gaussianKernelSize = gaussianKernelSize + 2;
                if (gaussianKernelSize > 15)
                {
                    gaussianKernelSize = 15;
                }
                prg_postProcessing->UseProgram();
                    prg_postProcessing->SetUniform("GaussianKernelSize", gaussianKernelSize);
                prg_postProcessing->UnUseProgram();
                std::cout << "Gaussian Kernel Size: " << gaussianKernelSize << std::endl;
                break;
            }

			case GLFW_KEY_M:
			{
				luminanceThreshold = luminanceThreshold + 0.05f;
				if (luminanceThreshold > 0.95f)
				{
					luminanceThreshold = 0.95f;
				}
				prg_postProcessing->UseProgram();
				    prg_postProcessing->SetUniform("LuminanceThreshold", luminanceThreshold);
				prg_postProcessing->UnUseProgram();
				std::cout << "Luminance threshold: " << luminanceThreshold << std::endl;
				break;
			}

			case GLFW_KEY_N:
			{
				luminanceThreshold = luminanceThreshold - 0.05f;
				if (luminanceThreshold < 0.05f)
				{
					luminanceThreshold = 0.05f;
				}
				prg_postProcessing->UseProgram();
				    prg_postProcessing->SetUniform("LuminanceThreshold", luminanceThreshold);
				prg_postProcessing->UnUseProgram();
				std::cout << "Luminance threshold: " << luminanceThreshold << std::endl;
				break;
			}

			case GLFW_KEY_O:
			{
				blurSteps = blurSteps + 1;
				std::cout << blurSteps << std::endl;
				break;
			}

            case GLFW_KEY_P:
            {
                blurSteps = blurSteps - 1;
                if (blurSteps < 1)
                {
                    blurSteps = 1;
                }
                std::cout << blurSteps << std::endl;
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
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
		sphere = std::make_shared<Sphere>(1.0f, 30, 30);
        camera = std::make_shared<Camera>();
		backgroundQuad = std::make_shared<BackgroundQuad>();

        // Textures
        tex_originalScene = std::make_shared<Texture>(GL_TEXTURE_2D, nullptr, m_windowDimensions.x, m_windowDimensions.y, 0, GL_RGBA, GL_RGBA, 0, 0, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);
		tex_thresholdedLuminance = std::make_shared<Texture>(GL_TEXTURE_2D, nullptr, m_windowDimensions.x, m_windowDimensions.y, 0, GL_RGBA, GL_RGBA, 0, 0, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);
		tex_thresholdedLuminanceVerticallyBlurred = std::make_shared<Texture>(GL_TEXTURE_2D, nullptr, m_windowDimensions.x, m_windowDimensions.y, 0, GL_RGBA, GL_RGBA, 0, 0, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);
		tex_thresholdedLuminanceVerticallyBlurredAndHorizontally = std::make_shared<Texture>(GL_TEXTURE_2D, nullptr, m_windowDimensions.x, m_windowDimensions.y, 0, GL_RGBA, GL_RGBA, 0, 0, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);
        
        // Framebuffer Objects
        fbo_originalScene = std::make_shared<FramebufferObject>();
        fbo_originalScene->AttachTexture2D(tex_originalScene->GetHandle());
        fbo_originalScene->AttachRenderbuffer(FramebufferObject::RenderBufferAttachement::DEPTH, GL_DEPTH_COMPONENT, m_windowDimensions.x, m_windowDimensions.y);
        fbo_originalScene->Validate();

		fbo_thresholdedLuminance = std::make_shared<FramebufferObject>();
		fbo_thresholdedLuminance->AttachTexture2D(tex_thresholdedLuminance->GetHandle());
		fbo_thresholdedLuminance->Validate();

		fbo_thresholdedLuminanceVerticallyBlurred = std::make_shared<FramebufferObject>();
		fbo_thresholdedLuminanceVerticallyBlurred->AttachTexture2D(tex_thresholdedLuminanceVerticallyBlurred->GetHandle());
		fbo_thresholdedLuminanceVerticallyBlurred->Validate();

        fbo_thresholdedLuminanceVerticallyBlurredAndHorizontally = std::make_shared<FramebufferObject>();
		fbo_thresholdedLuminanceVerticallyBlurredAndHorizontally->AttachTexture2D(tex_thresholdedLuminanceVerticallyBlurredAndHorizontally->GetHandle());
		fbo_thresholdedLuminanceVerticallyBlurredAndHorizontally->Validate();

        // Programs
        prg_lighting = std::make_shared<ShaderProgram>("../../resources/shaders/Lighting.vs", "../../resources/shaders/Lighting.fs");
        prg_postProcessing = std::make_shared<ShaderProgram>("../../resources/shaders/PostProcessing.vs", "../../resources/shaders/PostProcessing.fs");
        prg_postProcessing->UseProgram();
		    prg_postProcessing->SetUniform("LuminanceThreshold", luminanceThreshold);
            prg_postProcessing->SetUniform("GaussianKernelSize", gaussianKernelSize);
        prg_postProcessing->UnUseProgram();
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

        // Set view and projection matrix based on camera view
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->GetViewMatrix()));
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera->GetProjectionMatrix()));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        /*
         * 1. Render scene to screen
         */
        prg_lighting->UseProgram();
            glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ssi_phongIndex);
            skull->ResetModelMatrix();
			skull->ScaleModel(glm::vec3(0.005f, 0.005f, 0.005f));
			skull->RotateModel(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            prg_lighting->SetUniform("modelMatrix", skull->GetModelMatrix());
            skull->Render(GL_TRIANGLES);
			sphere->ResetModelMatrix();
			sphere->ScaleModel(glm::vec3(0.2f, 0.2f, 0.2f));
			sphere->TranslateModel(glm::vec3(0.0f, 1.5f, 0.0f));
			prg_lighting->SetUniform("modelMatrix", sphere->GetModelMatrix());
			sphere->Render(GL_TRIANGLES);
        prg_lighting->UnUseProgram();

        if (bloomEnabled)
        {
            /*
             * 2. Render scene to texture.
             * 
             * If bloom effect is enabled, as a first step, the scene
             * is rendered to a texture. A custom FBO is bound and both the 
             * color and depth buffer are cleared. Then, the subroutine uniform 
             * is set and the  scene is rendered to the texture attached to the 
             * custom FBO.
             */
            prg_lighting->UseProgram();
                glBindFramebuffer(GL_FRAMEBUFFER, fbo_originalScene->GetHandle());
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ssi_phongIndex);
                    prg_lighting->SetUniform("modelMatrix", skull->GetModelMatrix());
                    skull->Render(GL_TRIANGLES);
					prg_lighting->SetUniform("modelMatrix", sphere->GetModelMatrix());
					sphere->Render(GL_TRIANGLES);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            prg_lighting->UnUseProgram();

			// Reset view and projection matrix
			glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
				glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			/*
			 * 3. Compute thresholded luminance map
			 */
			prg_postProcessing->UseProgram();
				glBindFramebuffer(GL_FRAMEBUFFER, fbo_thresholdedLuminance->GetHandle());
					glClear(GL_COLOR_BUFFER_BIT);
					glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ssi_thresholdedLuminance);
					prg_postProcessing->SetUniform("modelMatrix", glm::mat4());
					glActiveTexture(GL_TEXTURE0);
						tex_originalScene->Bind();
							backgroundQuad->Render();
						tex_originalScene->Unbind();
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			prg_postProcessing->UnUseProgram();

            /*
             * 4. Gaussian blurring
             *
             * As a second step, the image is blurred vertically 
             * and the result is rendered to an intermediate texture 
             * and FBO. Next, the intermediate result is taken
             * and based on this, horizontal blurring is performed.
             * The result is then drawn to the default framebuffer.
             * The view and projection matrices have to be set
             * to identity. It is possible, to blur multiple times
             * in both directions using an intermediate FBO.
             */

            // Blur once vertically
            prg_postProcessing->UseProgram();
                glBindFramebuffer(GL_FRAMEBUFFER, fbo_thresholdedLuminanceVerticallyBlurred->GetHandle());
                    glClear(GL_COLOR_BUFFER_BIT);
                    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ssi_gaussianBlurVertical);
                    prg_postProcessing->SetUniform("modelMatrix", glm::mat4());
					glActiveTexture(GL_TEXTURE0);
						tex_thresholdedLuminance->Bind();
							backgroundQuad->Render();
						tex_thresholdedLuminance->Unbind();

                    // Check if vertical blurring should occur more than once
                    for (int i = 1; i < blurSteps; ++i)
                    {
						glActiveTexture(GL_TEXTURE0);
							tex_thresholdedLuminanceVerticallyBlurred->Bind();
								backgroundQuad->Render();
							tex_thresholdedLuminanceVerticallyBlurred->Unbind();
                    }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            prg_postProcessing->UnUseProgram();

            // Blur once horizontally
            prg_postProcessing->UseProgram();
                glBindFramebuffer(GL_FRAMEBUFFER, fbo_thresholdedLuminanceVerticallyBlurredAndHorizontally->GetHandle());
                    glClear(GL_COLOR_BUFFER_BIT);
                    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ssi_gaussianBlurHorizontal);
                    prg_postProcessing->SetUniform("modelMatrix", glm::mat4());
					glActiveTexture(GL_TEXTURE0);
						tex_thresholdedLuminanceVerticallyBlurred->Bind();
							backgroundQuad->Render();
						tex_thresholdedLuminanceVerticallyBlurred->Unbind();
            
                    // Check if horizontal blurring should occur more than once
                    for (int i = 1; i < blurSteps; ++i)
                    {
						glActiveTexture(GL_TEXTURE0);
							tex_thresholdedLuminanceVerticallyBlurredAndHorizontally->Bind();
								backgroundQuad->Render();
							tex_thresholdedLuminanceVerticallyBlurredAndHorizontally->Unbind();
                    }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            prg_postProcessing->UnUseProgram();

            // Render final output to screen
            prg_postProcessing->UseProgram();
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ssi_additiveBlending);
                prg_postProcessing->SetUniform("modelMatrix", glm::mat4());
				glActiveTexture(GL_TEXTURE0);
					tex_originalScene->Bind();
				glActiveTexture(GL_TEXTURE1);
					tex_thresholdedLuminanceVerticallyBlurredAndHorizontally->Bind();
						backgroundQuad->Render();
					tex_thresholdedLuminanceVerticallyBlurredAndHorizontally->Unbind();
				glActiveTexture(GL_TEXTURE0);
					tex_originalScene->Unbind();
            prg_postProcessing->UnUseProgram();
        }
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
    GLuint ssi_texelFetch{0};
    GLuint ssi_gaussianBlurVertical{3};
    GLuint ssi_gaussianBlurHorizontal{4};
	GLuint ssi_thresholdedLuminance{5};
	GLuint ssi_additiveBlending{6};

    std::shared_ptr<PLYMesh> skull{nullptr};
	std::shared_ptr<Sphere> sphere{nullptr};
    std::shared_ptr<Camera> camera{nullptr};
	std::shared_ptr<BackgroundQuad> backgroundQuad{ nullptr };

    std::shared_ptr<Texture> tex_originalScene{nullptr};
	std::shared_ptr<Texture> tex_thresholdedLuminance{ nullptr };
    std::shared_ptr<Texture> tex_thresholdedLuminanceVerticallyBlurred{nullptr};
    std::shared_ptr<Texture> tex_thresholdedLuminanceVerticallyBlurredAndHorizontally{nullptr};

    std::shared_ptr<FramebufferObject> fbo_originalScene{nullptr};
	std::shared_ptr<FramebufferObject> fbo_thresholdedLuminance{ nullptr };
    std::shared_ptr<FramebufferObject> fbo_thresholdedLuminanceVerticallyBlurred{nullptr};
    std::shared_ptr<FramebufferObject> fbo_thresholdedLuminanceVerticallyBlurredAndHorizontally{nullptr};

    std::shared_ptr<ShaderProgram> prg_lighting{nullptr};
    std::shared_ptr<ShaderProgram> prg_postProcessing{nullptr};

    bool bloomEnabled{false};
    int blurSteps{1};
    int gaussianKernelSize{3};
	float luminanceThreshold{0.8f};
};

int main(int argc, char ** argv)
{
    try
    {
        return SubroutineBloomEffect().Run();
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