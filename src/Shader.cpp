/****************************************************************************
 *                                                                          *
 * QRenderOrang - an IDE for GLSL shader authoring                          *
 *                                                                          *
 * copyright (c) 2008 Carlo Casta <carlo.casta@gmail.com>                   *
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
    : m_usable(false)
{
    m_shaderProgram = glCreateProgram();
}

Shader::~Shader()
{
    glDeleteProgram(m_shaderProgram);
}

void Shader::start()
{
    if (m_usable)
    {
        glUseProgram(m_shaderProgram);
    }
}

void Shader::stop()
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

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    {

        QByteArray tmpVertSource = m_vertSource.toLocal8Bit();
        const GLchar* vertSource = tmpVertSource.data();
        GLint vertSourceLen = tmpVertSource.length();

        glShaderSource(vertShader, 1, &vertSource, &vertSourceLen);
        glCompileShader(vertShader);
    }
    CheckError(vertShader, GL_COMPILE_STATUS);


    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    {
        QByteArray tmpFragSource = m_fragSource.toLocal8Bit();
        const GLchar* fragSource = tmpFragSource.data();
        GLint fragSourceLen = tmpFragSource.length();

        glShaderSource(fragShader, 1, &fragSource, &fragSourceLen);
        glCompileShader(fragShader);
    }
    CheckError(fragShader, GL_COMPILE_STATUS);

    glAttachShader(m_shaderProgram, vertShader);
    glAttachShader(m_shaderProgram, fragShader);
    glLinkProgram(m_shaderProgram);

    if (CheckError(m_shaderProgram, GL_LINK_STATUS))
    {
        m_compileLog = "OK";
        m_usable = true;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

bool Shader::CheckError(GLuint obj, GLuint check)
{
    GLint res;

    if (glIsProgram(obj))
    {
        glGetProgramiv(obj, check, &res);
    }
    else if (glIsShader(obj))
    {
        glGetShaderiv(obj, check, &res);
    }
    else
    {
        res = GL_FALSE;
    }

    if (res == GL_FALSE)
    {
        FillInfoLog(obj);
    }
    
    return (res == GL_TRUE);
}

void Shader::FillInfoLog(GLuint obj)
{
    GLint infologLength = 0;
    if (glIsProgram(obj))
    {
        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
    }
    else if (glIsShader(obj))
    {
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
    }
    
    m_compileLog = "FAILED\n";
    if (infologLength > 0)
    {
        QVector<char> infoLog(infologLength);
        if (glIsProgram(obj))
        {
            glGetProgramInfoLog(obj, infologLength, nullptr, &infoLog[0]);
        }
        else
        {
            glGetShaderInfoLog(obj, infologLength, nullptr, &infoLog[0]);
        }

        m_compileLog.append(&infoLog[0]);
    }
}
