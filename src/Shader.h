/****************************************************************************
 *                                                                          *
 * QRenderOrang - an IDE for GLSL shader authoring                          *
 *                                                                          *
 * Copyright (c) 2008 Carlo Casta <carlo.casta@gmail.com>                   *
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
#pragma once

#include <QVector>
#include <QList>
#include <QString>

#include "GL/glew.h"

class Shader
{
public:
    Shader();
    virtual ~Shader();
    void start();
    void stop();
    void compileAndLink();
    void setSource(const QString& v, const QString& f);
    const QString& compileLog();

private:
    bool CheckError(GLuint obj, GLuint check);
    void FillInfoLog(GLuint obj);

    QString m_vertSource;
    QString m_fragSource;
    QString m_compileLog;
    GLuint m_shaderProgram;
    bool m_shaderLinked;
};

inline void Shader::setSource(const QString &v, const QString &f)
{
    m_vertSource = v;
    m_fragSource = f;
}

inline const QString& Shader::compileLog()
{
    return m_compileLog;
}
