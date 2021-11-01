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

class GLFWApp
{
public:
    GLFWApp();
    virtual ~GLFWApp();

    virtual int Run();

protected:
	GLFWwindow* m_window{ nullptr };
	std::string m_windowName{ "GLFWApp" };
	bool        m_fullscreenWindow{ false };
	GLFWcursor* m_cursor{ nullptr };
	bool		m_createStandardCursor{ false };
    glm::uvec2  m_windowDimensions{glm::uvec2(800, 600)};
    glm::ivec2  m_windowPosition{glm::ivec2(20, 20)};
	glm::ivec2  m_cursorPosition{ glm::ivec2(100, 100) };
    int64_t     m_currentFrameID{0};

    // GLFW window loop functions
    auto virtual Setup()				 -> void;
    auto virtual PreCreate()			 -> void;
	auto virtual CreateRenderingTarget() -> void;
    auto virtual PostCreate()		     -> void;
    auto virtual InitGL()				 -> void;
    auto virtual Update()				 -> void;
    auto virtual Draw()					 -> void = 0;
    auto virtual FinishFrame()			 -> void;
    auto virtual ShutdownGL()			 -> void;
    auto virtual Cleanup()				 -> void;

	// GLFW input callbacks
	auto virtual OnKey(int key, int scancode, int action, int mods) -> void;
	auto virtual OnCharacter(unsigned int code_point)               -> void;
	auto virtual OnCharacterMods(unsigned int codepoint, int mods)  -> void;
	auto virtual OnMouseButton(int button, int action, int mods)    -> void;
	auto virtual OnMouseMove(double x, double y)                    -> void;
	auto virtual OnMouseEnter(int entered)                          -> void;
	auto virtual OnScroll(double x, double y)                       -> void;
	auto virtual OnDrop(int count, const char **paths)				-> void;

	// GLFW window callbacks
	auto virtual OnWindowResize(int width, int heigth)              -> void;
	auto virtual OnWindowRepositioning(int x, int y)                -> void;
	auto virtual OnWindowClose()                                    -> void;
	auto virtual OnWindowRefresh()									-> void;
	auto virtual OnWindowFocus(int focused)							-> void;
	auto virtual OnWindowIconify(int iconified)						-> void;

private:
    // GLFW input callback functions
    friend void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	friend void CharacterCallback(GLFWwindow* window, unsigned int codepoint);
	friend void CharacterModsCallback(GLFWwindow* window, unsigned int codepoint, int mods);
    friend void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    friend void MouseMoveCallback(GLFWwindow* window, double x, double y);
	friend void CursorEnterCallback(GLFWwindow* window, int enter);
    friend void ScrollCallback(GLFWwindow* window, double x, double y);
	friend void DropCallback(GLFWwindow* window, int count, const char **paths);

	// GLFW window callback functions
    friend void WindowResizeCallback(GLFWwindow* window, int width, int height);
    friend void WindowRepositioningCallback(GLFWwindow* window, int xpos, int ypos);
	friend void WindowCloseCallback(GLFWwindow* window);
	friend void WindowRefreshCallback(GLFWwindow* window);
	friend void WindowFocusCallback(GLFWwindow* window, int focused);
	friend void WindowIconifyCallback(GLFWwindow* window, int iconified);
};
