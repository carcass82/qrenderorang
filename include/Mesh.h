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
#include "PreviewWidget.h"

class Mesh
{
public:
    enum MeshType { CUBE, SPHERE };

    Mesh* Load(MeshType Shape);
    Mesh* Load(const QString& filePath);

    Vertex* vertices();
    uint32_t* indices();
    size_t numVertices() const;
    size_t numIndices() const;
    size_t numTriangles() const;
    const vec3& center() const;
    const vec3& size() const;

    static QString typeToString(Mesh::MeshType type);

private:
    void ComputeNormals();
    void ComputeBoundingBox();
    void ComputeTangentsAndBitangents();

    vec3 m_MeshCenter;
    vec3 m_MeshSize;
    QVector<Vertex> m_Vertices;
    QVector<uint32_t> m_Indices;
};

inline Vertex* Mesh::vertices()
{
    return &m_Vertices[0];
}

inline uint32_t* Mesh::indices()
{
    return &m_Indices[0];
}

inline size_t Mesh::numVertices() const
{
    return m_Vertices.size();
}

inline size_t Mesh::numIndices() const
{
    return m_Indices.size();
}

inline size_t Mesh::numTriangles() const
{
    return m_Indices.size() / 3;
}

inline const vec3& Mesh::center() const
{
    return m_MeshCenter;
}

inline const vec3& Mesh::size() const
{
    return m_MeshSize;
}

inline QString Mesh::typeToString(Mesh::MeshType type)
{
    switch (type)
    {
    case MeshType::CUBE:   return "Cube (builtin)";
    case MeshType::SPHERE: return "Sphere (builtin)";
    default:               return "";
    }
}
