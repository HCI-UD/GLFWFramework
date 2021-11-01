#include "Common.h"
#include <opencv2/opencv.hpp>

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

class SubroutineGaussianBlurOpenCVBackgroundTexture : public GLFWApp
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
                blurringEnabled = !blurringEnabled;
                break;
            }

            case GLFW_KEY_O:
            {
                blurSteps = blurSteps + 1;
                std::cout << blurSteps << std::endl;
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

    // GLFW window loop functions
    auto virtual Setup() -> void override
    {
        m_image = cv::imread("../../resources/images/mini.jpg", CV_LOAD_IMAGE_COLOR);
        cv::flip(m_image, m_image, 0);
        m_windowDimensions.x = m_image.cols;
        m_windowDimensions.y = m_image.rows;
    }

    auto virtual InitGL() -> void override
    {
        // OpenGL state
        glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

        // UBO Matrices (binding index 0)
        ubo_matrices = GLUtils::BufferUtils::CreateBufferObject(GL_UNIFORM_BUFFER, nullptr, 2 * sizeof(glm::mat4), GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_matrices);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::mat4()));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Pointers
		backgroundQuad = std::make_shared<BackgroundQuad>();

        // Textures
        tex_background = std::make_shared<Texture>(GL_TEXTURE_2D, m_image.data, m_windowDimensions.x, m_windowDimensions.y);
        tex_verticallyBlurredScene = std::make_shared<Texture>(GL_TEXTURE_2D, nullptr, m_windowDimensions.x, m_windowDimensions.y, 0, GL_RGBA, GL_RGBA, 0, 0, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);
        tex_verticallyAndHorizontallyBlurredScene = std::make_shared<Texture>(GL_TEXTURE_2D, nullptr, m_windowDimensions.x, m_windowDimensions.y, 0, GL_RGBA, GL_RGBA, 0, 0, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);
        
		// Framebuffer Objects
		fbo_verticallyBlurredScene = std::make_shared<FramebufferObject>();
		fbo_verticallyBlurredScene->AttachTexture2D(tex_verticallyBlurredScene->GetHandle());
		fbo_verticallyBlurredScene->Validate();

		fbo_verticallyAndHorizontallyBlurredScene = std::make_shared<FramebufferObject>();
		fbo_verticallyAndHorizontallyBlurredScene->AttachTexture2D(tex_verticallyAndHorizontallyBlurredScene->GetHandle());
		fbo_verticallyAndHorizontallyBlurredScene->Validate();

        // Programs
        prg_postProcessing = std::make_shared<ShaderProgram>("../../resources/shaders/PostProcessing.vs", "../../resources/shaders/PostProcessing.fs");
        prg_postProcessing->UseProgram();
            prg_postProcessing->SetUniform("GaussianKernelSize", gaussianKernelSize);
        prg_postProcessing->UnUseProgram();
    }

    auto virtual Draw() -> void override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*
         * 1. Render OpenCV image as background texture
         */
        prg_postProcessing->UseProgram();
            glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ssi_texelFetch);
            prg_postProcessing->SetUniform("modelMatrix", glm::mat4());
			glActiveTexture(GL_TEXTURE0);
				tex_background->Bind();
					backgroundQuad->Render();
				tex_background->Unbind();
        prg_postProcessing->UnUseProgram();

        /*
         * 2. Gaussian blurring
         *
         * If gaussian blurring is enabled, as a first step
         * the OpenCV image is blurred vertically and the result is
         * rendered to an intermediate texture and FBO. In
         * the second step, the intermediate result is taken
         * and based on this, horizontal blurring is performed.
         * It is possible, to blur multiple times in both directions 
         * using an intermediate FBO.
         */
        if (blurringEnabled)
        {
            // Blur once vertically
            prg_postProcessing->UseProgram();
                glBindFramebuffer(GL_FRAMEBUFFER, fbo_verticallyBlurredScene->GetHandle());
                    glClear(GL_COLOR_BUFFER_BIT);
                    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ssi_gaussianBlurVertical);
                    prg_postProcessing->SetUniform("modelMatrix", glm::mat4());
					glActiveTexture(GL_TEXTURE0);
						tex_background->Bind();
							backgroundQuad->Render();
						tex_background->Unbind();

                    // Check if vertical blurring should occur more than once
                    for (int i = 1; i < blurSteps; ++i)
                    {
						glActiveTexture(GL_TEXTURE0);
							tex_verticallyBlurredScene->Bind();
								backgroundQuad->Render();
							tex_verticallyBlurredScene->Unbind();
                    }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            prg_postProcessing->UnUseProgram();

            // Blur once horizontally
            prg_postProcessing->UseProgram();
                glBindFramebuffer(GL_FRAMEBUFFER, fbo_verticallyAndHorizontallyBlurredScene->GetHandle());
                    glClear(GL_COLOR_BUFFER_BIT);
                    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ssi_gaussianBlurHorizontal);
                    prg_postProcessing->SetUniform("modelMatrix", glm::mat4());
					glActiveTexture(GL_TEXTURE0);
						tex_verticallyBlurredScene->Bind();
							backgroundQuad->Render();
						tex_verticallyBlurredScene->Unbind();

                    // Check if horizontal blurring should occur more than once
                    for (int i = 1; i < blurSteps; ++i)
                    {
						glActiveTexture(GL_TEXTURE0);
							tex_verticallyAndHorizontallyBlurredScene->Bind();
								backgroundQuad->Render();
							tex_verticallyAndHorizontallyBlurredScene->Unbind();
                    }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            prg_postProcessing->UnUseProgram();

            // Render final output to screen
            prg_postProcessing->UseProgram();
                glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ssi_texelFetch);
                prg_postProcessing->SetUniform("modelMatrix", glm::mat4());
				glActiveTexture(GL_TEXTURE0);
					tex_verticallyAndHorizontallyBlurredScene->Bind();
						backgroundQuad->Render();
					tex_verticallyAndHorizontallyBlurredScene->Unbind();
            prg_postProcessing->UnUseProgram();
        }
    }

    auto virtual ShutdownGL() -> void override
    {
        GLUtils::BufferUtils::DeleteBufferObject(&ubo_matrices);
    }

private:
    cv::Mat m_image;

    GLuint ubo_matrices{0};

    GLuint ssi_texelFetch{0};
    GLuint ssi_gaussianBlurVertical{3};
    GLuint ssi_gaussianBlurHorizontal{4};

	std::shared_ptr<BackgroundQuad> backgroundQuad{ nullptr };

    std::shared_ptr<Texture> tex_background{nullptr};
    std::shared_ptr<Texture> tex_verticallyBlurredScene{nullptr};
    std::shared_ptr<Texture> tex_verticallyAndHorizontallyBlurredScene{nullptr};

	std::shared_ptr<FramebufferObject> fbo_verticallyBlurredScene{ nullptr };
	std::shared_ptr<FramebufferObject> fbo_verticallyAndHorizontallyBlurredScene{ nullptr };

    std::shared_ptr<ShaderProgram> prg_postProcessing{nullptr};

    bool blurringEnabled{false};
    int blurSteps{1};
    int gaussianKernelSize{3};
};

int main(int argc, char ** argv)
{
    try
    {
        return SubroutineGaussianBlurOpenCVBackgroundTexture().Run();
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