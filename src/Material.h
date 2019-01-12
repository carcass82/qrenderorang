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
#include <glm/vec4.hpp>

#include "Shader.h"

class Material
{
public:
    struct MaterialProperties
    {
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        glm::vec4 emission;
        float shininess;
    };

    Material();
    ~Material();

    uint32_t passCount() const;
    void BeginDraw(int pass = 0);
    void EndDraw();

    void SetShader(Shader* shader, int pass = 0);
    void SetMaterialProperties(const MaterialProperties& properties);

private:
    int m_currentPass = 0;
    MaterialProperties m_Props;
    QVector<Shader*> m_Shaders;
};

inline uint32_t Material::passCount() const
{
    return m_Shaders.size();
}

inline void Material::SetMaterialProperties(const MaterialProperties& properties)
{
    m_Props = properties;
}

inline void Material::SetShader(Shader* shader, int pass)
{
    if (m_Shaders.empty())
    {
        m_Shaders.push_back(shader);
    }
    else if (pass < m_Shaders.size())
    {
        delete m_Shaders[pass];
        m_Shaders[pass] = shader;
    }
}
