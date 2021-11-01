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

ShaderProgram::ShaderProgram()
{
    m_handle = glCreateProgram();
}

ShaderProgram::ShaderProgram(std::string filenameVertexShader, std::string filenameFragmentShader)
{
    m_handle = glCreateProgram();

    try
    {
        AttachShader(filenameVertexShader, GL_VERTEX_SHADER);
        AttachShader(filenameFragmentShader, GL_FRAGMENT_SHADER);
        LinkProgram();
    }
    catch (std::runtime_error & err)
    {
        std::cout << err.what() << std::endl;
    }
}

ShaderProgram::~ShaderProgram(void)
{
    if (m_handle == 0) return;

	// Get the shader handles
    GLuint *shaderHandles = new GLuint[m_shaderCount];
    glGetAttachedShaders(m_handle, m_shaderCount, nullptr, shaderHandles);

	// Delete the shaders
    for (unsigned int i = 0; i < m_shaderCount; i++)
		glDeleteShader(shaderHandles[i]);

	// Delete the program
    glDeleteProgram(m_handle);

	// Delete shader handles
	delete[] shaderHandles;
}

// Getter
auto ShaderProgram::GetProgramHandle() const -> const GLuint
{
	return m_handle;
}

auto ShaderProgram::GetShaderCount() const -> const GLuint
{
    return m_shaderCount;
}

// Shader Usage and Setup
auto ShaderProgram::UseProgram() const -> void
{
    glUseProgram(m_handle);
}

auto ShaderProgram::UnUseProgram() const -> void
{
    glUseProgram(0);
}

auto ShaderProgram::LinkProgram() const -> void
{
    GLint programLinked{0};

    if (m_shaderCount >= 2)
    {
        // Link program and check for errors
        glLinkProgram(m_handle);
        glGetProgramiv(m_handle, GL_LINK_STATUS, &programLinked);

        // Print program info log and throw exception if the program couldn't be linked correctly
        if (!programLinked)
        {
            try
            {
                PrintProgramInfoLog(m_handle);
            }
            catch (std::runtime_error & err)
            {
                std::cout << err.what() << std::endl;
            }

            std::string error{"Program was not linked correctly"};
            std::cerr << error << std::endl;
            throw std::runtime_error(error.c_str());
        }
    }
    else
    {
        // Throw exception if not at least two shaders are attached to the program
        std::string error{"There must be at least 2 shaders (vertex + fragment) attached to the program!"};
        std::cerr << error << std::endl;
        throw std::runtime_error(error.c_str());
    }
}

auto ShaderProgram::AttachShader(std::string filename, GLenum type) -> void
{
	// 1. Read shader string from file
	const std::string str = ReadShaderFromFile(filename);
	const GLchar *shaderString = str.c_str();
	
	// 2. Create shader
	GLuint shaderHandle{0};
	GLint shaderCompiled{0};

	shaderHandle = glCreateShader(type);
	glShaderSource(shaderHandle, 1, static_cast<const GLchar**>(&shaderString), nullptr);

	glCompileShader(shaderHandle);
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &shaderCompiled);

	// Print shader info log and throw exception if the shader couldn't be compiled correctly
	if(!shaderCompiled)
	{
        try
        {
            PrintShaderInfoLog(shaderHandle);
        }
        catch (std::runtime_error & err)
        {
            std::cout << err.what() << std::endl;
        }

        std::string error{"Shader couldn't be compiled correctly"};
        std::cerr << error << std::endl;
        throw std::runtime_error(error.c_str());
	}

	// Attach shader
    glAttachShader(m_handle, shaderHandle);
    m_shaderCount++;
}

auto ShaderProgram::ReadShaderFromFile(const std::string filename) -> const std::string
{
  std::ifstream shaderFile(filename.c_str());

  // Find the file size
  shaderFile.seekg(0,std::ios::end);
  std::streampos length = shaderFile.tellg();
  shaderFile.seekg(0,std::ios::beg);

  // Read whole file into a vector:
  std::vector<char> buffer(length);
  shaderFile.read(&buffer[0],length);

  // Return the shader string
  return std::string(buffer.begin(), buffer.end());
}

auto ShaderProgram::PrintShaderInfoLog(GLuint shader) -> void
{
	int info_log_length{0};
	int chars_written{0};
	GLchar *infoLog{nullptr};

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

	if(info_log_length > 0)
	{
		infoLog = static_cast<GLchar *>(malloc(info_log_length));
		
		if(infoLog == nullptr)
		{
            std::string error{"Could not allocate Shader InfoLog buffer"};
            std::cerr << error << std::endl;
            throw std::runtime_error(error.c_str());
		}

		glGetShaderInfoLog(shader, info_log_length, &chars_written, infoLog);
		printf("Shader InfoLog:\n%s\n\n", infoLog);
		free(infoLog);
	}
}

auto ShaderProgram::PrintProgramInfoLog(GLuint program) -> void
{
	int info_log_length{0};
	int chars_written{0};
	GLchar *infoLog{nullptr};

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);

	if(info_log_length > 0)
	{
		infoLog = static_cast<GLchar *>(malloc(info_log_length));
		
		if(infoLog == nullptr)
		{
            std::string error{"Could not allocate Program InfoLog buffer"};
            std::cerr << error << std::endl;
            throw std::runtime_error(error.c_str());
		}

		glGetProgramInfoLog(program, info_log_length, &chars_written, infoLog);
		printf("Program InfoLog:\n%s\n\n", infoLog);
		free(infoLog);
	}
}

auto ShaderProgram::GetUniformLocation(const GLchar *name)->GLint
{
	std::map<std::string, int>::iterator pos;
	pos = m_uniformLocations.find(name);

    if (pos == m_uniformLocations.end())
	{
        m_uniformLocations[name] = glGetUniformLocation(m_handle, name);
	}

    return m_uniformLocations[name];
}

// Uniform location setting
void ShaderProgram::SetUniform(const char *name, float x, float y, float z)
{
	GLint location = GetUniformLocation(name);
	glUniform3f(location, x, y, z);
}

auto ShaderProgram::SetUniform( const char *name, const glm::vec2 &v) -> void
{
	GLint location = GetUniformLocation(name);
	glUniform2f(location, v.x, v.y);
}

auto ShaderProgram::SetUniform(const char *name, const glm::vec3 &v) -> void
{
	this->SetUniform(name, v.x, v.y, v.z);
}

auto ShaderProgram::SetUniform(const char *name, const glm::vec4 &v) -> void
{
	GLint location = GetUniformLocation(name);
	glUniform4f(location, v.x, v.y, v.z, v.w);
}

auto ShaderProgram::SetUniform(const char *name, const glm::mat4 &m) -> void
{
	GLint location = GetUniformLocation(name);
	glUniformMatrix4fv(location, 1, GL_FALSE, &m[0][0]);
}

auto ShaderProgram::SetUniform(const char *name, const glm::mat3 &m) -> void
{
	GLint location = GetUniformLocation(name);
	glUniformMatrix3fv(location, 1, GL_FALSE, &m[0][0]);
}

auto ShaderProgram::SetUniform(const char *name, float val) -> void
{
	GLint location = GetUniformLocation(name);
	glUniform1f(location, val);
}

auto ShaderProgram::SetUniform(const char *name, int val) -> void
{
	GLint location = GetUniformLocation(name);
	glUniform1i(location, val);
}

auto ShaderProgram::SetUniform(const char *name, GLuint val) -> void
{
	GLint location = GetUniformLocation(name);
	glUniform1ui(location, val);
}

auto ShaderProgram::SetUniform(const char *name, bool val) -> void
{
	int location = GetUniformLocation(name);
	glUniform1i(location, val);
}
