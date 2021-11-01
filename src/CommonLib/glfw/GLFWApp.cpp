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

// GLFW input callback functions
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    GLFWApp* instance = static_cast<GLFWApp *>(glfwGetWindowUserPointer(window));
    instance->OnKey(key, scancode, action, mods);
}

void CharacterCallback(GLFWwindow* window, unsigned int codepoint)
{
	GLFWApp* instance = static_cast<GLFWApp *>(glfwGetWindowUserPointer(window));
	instance->OnCharacter(codepoint);
}

void CharacterModsCallback(GLFWwindow* window, unsigned int codepoint, int mods)
{
	GLFWApp* instance = static_cast<GLFWApp *>(glfwGetWindowUserPointer(window));
	instance->OnCharacterMods(codepoint, mods);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    GLFWApp* instance = static_cast<GLFWApp *>(glfwGetWindowUserPointer(window));
    instance->OnMouseButton(button, action, mods);
}

void MouseMoveCallback(GLFWwindow* window, double x, double y)
{
    GLFWApp* instance = static_cast<GLFWApp *>(glfwGetWindowUserPointer(window));
    instance->OnMouseMove(x, y);
}

void CursorEnterCallback(GLFWwindow* window, int enter)
{
	GLFWApp* instance = static_cast<GLFWApp *>(glfwGetWindowUserPointer(window));
	instance->OnMouseEnter(enter);
}

void ScrollCallback(GLFWwindow* window, double x, double y)
{
    GLFWApp* instance = static_cast<GLFWApp *>(glfwGetWindowUserPointer(window));
    instance->OnScroll(x, y);
}

void DropCallback(GLFWwindow* window, int count, const char **paths)
{
	GLFWApp* instance = static_cast<GLFWApp *>(glfwGetWindowUserPointer(window));
	instance->OnDrop(count, paths);
}

// GLFW window callback functions
void WindowResizeCallback(GLFWwindow* window, int width, int height)
{
    GLFWApp* instance = static_cast<GLFWApp *>(glfwGetWindowUserPointer(window));
    instance->OnWindowResize(width, height);
}

void WindowRepositioningCallback(GLFWwindow* window, int xpos, int ypos)
{
    GLFWApp* instance = static_cast<GLFWApp *>(glfwGetWindowUserPointer(window));
    instance->OnWindowRepositioning(xpos, ypos);
}

void WindowCloseCallback(GLFWwindow* window)
{
	GLFWApp* instance = static_cast<GLFWApp *>(glfwGetWindowUserPointer(window));
	instance->OnWindowClose();
}

void WindowRefreshCallback(GLFWwindow* window)
{
	GLFWApp* instance = static_cast<GLFWApp *>(glfwGetWindowUserPointer(window));
	instance->OnWindowRefresh();
}

void WindowFocusCallback(GLFWwindow* window, int focused)
{
	GLFWApp* instance = static_cast<GLFWApp *>(glfwGetWindowUserPointer(window));
	instance->OnWindowFocus(focused);
}

void WindowIconifyCallback(GLFWwindow* window, int iconified)
{
	GLFWApp* instance = static_cast<GLFWApp *>(glfwGetWindowUserPointer(window));
	instance->OnWindowIconify(iconified);
}

// GLFW error callback function
void ErrorCallback(int errorCode, const char* errorDescription)
{
    std::string errorMessage{errorDescription};
    std::cerr << "Error code: " << errorCode << " - Error description: " << errorMessage << std::endl;
    throw std::runtime_error(errorMessage.c_str());
}

// GLFW input callbacks
auto GLFWApp::OnKey(int key, int scancode, int action, int mods) -> void
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
    }
}

auto GLFWApp::OnCharacter(unsigned int code_point) -> void
{
}

auto GLFWApp::OnCharacterMods(unsigned int codepoint, int mods) -> void
{
}

auto GLFWApp::OnMouseButton(int button, int action, int mods) -> void
{
}

auto GLFWApp::OnMouseMove(double x, double y) -> void
{
	m_cursorPosition = glm::ivec2(x, y);
}

auto GLFWApp::OnMouseEnter(int entered) -> void
{
}

auto GLFWApp::OnScroll(double x, double y) -> void
{
}

auto GLFWApp::OnDrop(int count, const char ** paths) -> void
{
}

// GLFW window callbacks
auto GLFWApp::OnWindowResize(int width, int heigth) -> void
{
	m_windowDimensions = glm::uvec2(width, heigth);
	//std::cout << "New window dimensions: (" << m_windowDimensions.x << ", " << m_windowDimensions.y << ")" << std::endl;
    glViewport(0, 0, width, heigth);
}

auto GLFWApp::OnWindowRepositioning(int x, int y) -> void
{
    m_windowPosition = glm::ivec2(x, y);
	//std::cout << "New window position: (" << x << ", " << y << ")" << std::endl;
}

auto GLFWApp::OnWindowClose() -> void
{
}

auto GLFWApp::OnWindowRefresh() -> void
{
}

auto GLFWApp::OnWindowIconify(int iconified) -> void
{
	/*if (iconified)
	{
		std::cout << "Window minimized." << std::endl;
	}
	else
	{
		std::cout << "Window maximized." << std::endl;
	}*/
}

auto GLFWApp::OnWindowFocus(int focused) -> void
{
	/*if (focused > 0)
	{
		std::cout << "Window in focus." << std::endl;
	}
	else
	{
		std::cout << "Window out of focus." << std::endl;
	}*/
}

// Constructor, Destructor and Run method
GLFWApp::GLFWApp()
{
	// Initialize the GLFW system for creating and positioning windows
	if (!glfwInit())
	{
		std::string error{ "Failed to initialize GLFW" };
		std::cerr << error << std::endl;
		throw std::runtime_error(error.c_str());
	}
	glfwSetErrorCallback(ErrorCallback);
}

GLFWApp::~GLFWApp()
{
	glfwDestroyCursor(m_cursor);
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

int GLFWApp::Run()
{
	try
	{
		Setup();

		PreCreate();
		CreateRenderingTarget();
		PostCreate();
		glGetError();

		InitGL();

		while (!glfwWindowShouldClose(m_window))
		{

			glfwPollEvents();
			++m_currentFrameID;
			Update();
			Draw();
			FinishFrame();
		}

		ShutdownGL();

		Cleanup();
	}
	catch (std::runtime_error & err)
	{
		std::cout << err.what() << std::endl;
	}

	return 0;
}

// GLFW window loop functions
auto GLFWApp::Setup() -> void
{
}

auto GLFWApp::PreCreate() -> void
{
	glfwWindowHint(GLFW_DECORATED, GL_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 16);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);
}

auto GLFWApp::CreateRenderingTarget() -> void
{
	if (m_fullscreenWindow)
	{
		m_window = glfwCreateWindow(m_windowDimensions.x, m_windowDimensions.y, m_windowName.c_str(), glfwGetPrimaryMonitor(), nullptr);
		
		if (!m_window)
		{
			std::string error{ "Unable to create OpenGL window" };
			std::cerr << error << std::endl;
			throw std::runtime_error(error.c_str());
		}
	}
	else
	{
		m_window = glfwCreateWindow(m_windowDimensions.x, m_windowDimensions.y, m_windowName.c_str(), nullptr, nullptr);
		
		if (!m_window)
		{
			std::string error{ "Unable to create rendering window" };
			std::cerr << error << std::endl;
			throw std::runtime_error(error.c_str());
		}
		if ((m_windowPosition.x > INT_MIN) && (m_windowPosition.y > INT_MIN))
		{
			glfwSetWindowPos(m_window, m_windowPosition.x, m_windowPosition.y);
		}
	}
}

auto GLFWApp::PostCreate() -> void
{
	// Window context & user pointer
    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this);

	// Input callbacks
    glfwSetKeyCallback(m_window, KeyCallback);
	glfwSetCharCallback(m_window, CharacterCallback);
	glfwSetCharModsCallback(m_window, CharacterModsCallback);
    glfwSetMouseButtonCallback(m_window, MouseButtonCallback);
    glfwSetCursorPosCallback(m_window, MouseMoveCallback);
    glfwSetCursorEnterCallback(m_window, CursorEnterCallback);
    glfwSetScrollCallback(m_window, ScrollCallback);
	glfwSetDropCallback(m_window, DropCallback);

	// Window callbacks
    glfwSetWindowSizeCallback(m_window, WindowResizeCallback);
    glfwSetWindowPosCallback(m_window, WindowRepositioningCallback);
	glfwSetWindowCloseCallback(m_window, WindowCloseCallback);
	glfwSetWindowRefreshCallback(m_window, WindowRefreshCallback);
	glfwSetWindowFocusCallback(m_window, WindowFocusCallback);
	glfwSetWindowIconifyCallback(m_window, WindowIconifyCallback);

	// Window cursor
	if (m_createStandardCursor)
	{
		m_cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
	}
	else
	{
		cv::Mat mat_cursor = cv::imread("..\\..\\resources\\cursors\\cursor_crosshair_red.png", -1);
		cv::cvtColor(mat_cursor, mat_cursor, CV_BGRA2RGBA);

		GLFWimage cursorImage;
		cursorImage.width  = mat_cursor.cols;
		cursorImage.height = mat_cursor.rows;
		cursorImage.pixels = mat_cursor.data;

		glm::ivec2 cursorCenter = glm::ivec2(mat_cursor.cols / 2, mat_cursor.rows / 2);

		m_cursor = glfwCreateCursor(&cursorImage, cursorCenter.x, cursorCenter.y);
	}
	glfwSetCursor(m_window, m_cursor);

	// Swap interval
	glfwSwapInterval(1);

    // Initialize OpenGL bindings
    // For some reason we have to set this expermiental flag to properly init 
    // GLEW  if we use a core context. After GLEW initialization, clear the 
    // OpenGL error variable as meaningless errors might occur during this step.
    glewExperimental = GL_TRUE;
    GLenum glewInitResult = glewInit();
    if (glewInitResult != GLEW_OK)
    {
        const GLubyte* glewError = glewGetErrorString(glewInitResult);
        std::string error{"Failed to initialize GLEW"};
        std::cerr << error << ": " << glewError << std::endl;
        throw std::runtime_error(error.c_str());
    }
    glGetError();
}

auto GLFWApp::InitGL() -> void
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

auto GLFWApp::Update() -> void
{
}

auto GLFWApp::FinishFrame() -> void
{	
    glfwSwapBuffers(m_window);

    // Error checking
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("OpenGL error %08x\n", err);
    }
}

auto GLFWApp::ShutdownGL() -> void
{
}

auto GLFWApp::Cleanup() -> void
{
}