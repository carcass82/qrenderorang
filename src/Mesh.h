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

#include <QString>
#include <QVector>

#include "GL/glew.h"

#include "Material.h"

#include "cclib/cclib.h"
using cc::math::vec2;
using cc::math::vec3;

class Mesh
{
public:
    enum MeshType { CUBE, SPHERE };

    Mesh();
    ~Mesh();
 	void Draw();
    void Load(MeshType Shape);
    void Load(const QString& filePath);

    Material* GetMaterial() const;
    void SetMaterial(Material* material);

    size_t numVertices() const;
    size_t numIndices() const;
    const vec3& center() const;
    const vec3& size() const;

    static QString TypeToString(Mesh::MeshType type);

private:
    void ComputeNormals();
    void ComputeBoundingBox();
    void ComputeTangentsAndBitangents();
    void SetupBuffers();

    GLuint m_VBuffer;
    GLuint m_IBuffer;

    struct Vertex
    {
        vec3 pos;
        vec3 normal;
        vec2 uv0;
        vec3 tangent;
        vec3 bitangent;
    };

    vec3 m_MeshCenter;
    vec3 m_MeshSize;
    QVector<Vertex> m_Vertices;
    QVector<uint32_t> m_Indices;
    Material* m_Material = nullptr;
};


inline Material* Mesh::GetMaterial() const
{
    return m_Material;
}

inline void Mesh::SetMaterial(Material* material)
{
    delete m_Material;
    m_Material = material;
}

inline size_t Mesh::numVertices() const
{
    return m_Vertices.size();
}

inline size_t Mesh::numIndices() const
{
    return m_Indices.size();
}

inline const vec3& Mesh::center() const
{
    return m_MeshCenter;
}

inline const vec3& Mesh::size() const
{
    return m_MeshSize;
}

inline QString Mesh::TypeToString(Mesh::MeshType type)
{
    switch (type)
    {
    case MeshType::CUBE:   return "Cube (builtin)";
    case MeshType::SPHERE: return "Sphere (builtin)";
    }

    return "";
}
