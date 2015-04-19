/****************************************************************************
 *                                                                          *
 * QRenderOrang - an IDE for GLSL shader authoring                          *
 *                                                                          *
 * copyright (c) 2008 Carlo Casta <ccasta@users.sourceforge.net>            *
 *                                                                          *
 * This program is free software; you can redistribute it and/or            *
 * modify it under the terms of the GNU General Public License              *
 * as published by the Free Software Foundation; either version 2           *
 * of the License, or (at your option) any later version.                   *
 *                                                                          *
 * This program is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU General Public License        *
 * along with this program; if not, write to                                *
 *                                                                          *
 * the Free Software Foundation, Inc.                                       *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA             *
 *                                                                          *
 ****************************************************************************/

#include "Shader.h"

Shader::Shader()
    : m_totPasses(0)
    , m_curPass(0)
    , m_usable(false)
{
    m_shaderProgram = glCreateProgram();
}

Shader::~Shader()
{
    glDeleteProgram(m_shaderProgram);
}

void Shader::render(int pass)
{
    Q_UNUSED(pass)

    if (m_usable)
    {
        glUseProgram(m_shaderProgram);
    }
}

void Shader::disable()
{
    glUseProgram(0);
}

void Shader::compileAndLink()
{
    if (m_vertSource.isEmpty() || m_fragSource.isEmpty())
    {
        m_compileLog = "OK (No sources)";
        return;
    }

    const GLchar* vertSource = m_vertSource.toLocal8Bit().data();
    GLint vertSourceLen = m_vertSource.length();

    // compile vertex shader
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertSource, &vertSourceLen);
    glCompileShader(vertShader);

    const GLchar* fragSource = m_fragSource.toLocal8Bit().data();
    GLint fragSourceLen = m_fragSource.length();

    // compile fragment shader
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSource, &fragSourceLen);
    glCompileShader(fragShader);

    glAttachShader(m_shaderProgram, vertShader);
    glAttachShader(m_shaderProgram, fragShader);
    glLinkProgram(m_shaderProgram);

    GLint res;
    glGetShaderiv(m_shaderProgram, GL_LINK_STATUS, &res);
    if (res == GL_FALSE)
    {
        FillInfoLog(m_shaderProgram);
    }
    else
    {
        m_compileLog = "OK";

        // shader link successful. update uniform locations
        // and load textures
        m_totPasses = 1;
        m_usable = true;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void Shader::FillInfoLog(GLuint obj)
{
    GLint infologLength = 0;
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0)
    {
        char* infoLog = new char[infologLength];
        glGetShaderInfoLog(obj, infologLength, nullptr, infoLog);

        m_compileLog = "FAILED\n";
        m_compileLog.append(infoLog);

        delete[] infoLog;
    }
}
