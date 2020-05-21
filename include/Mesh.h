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
#include <QJsonObject>
#include "PreviewWidget.h"

class Mesh
{
public:
    enum MeshType { CUBE, SPHERE, CUSTOM, MAX };

    Mesh* Load(MeshType Shape);
    Mesh* Load(const QString& filePath);

    Vertex* vertices();
    uint32_t* indices();
    size_t numVertices() const;
    size_t numIndices() const;
    size_t numTriangles() const;
    const vec3& center() const;
    const vec3& size() const;

    QJsonObject save() const;
    void load(const QJsonObject& data);

    bool valid() const;
    MeshType type() const;
    static QString typeToString(MeshType type);

private:
    void ComputeNormals();
    void ComputeBoundingBox();
    void ComputeTangentsAndBitangents();

    vec3 m_MeshCenter;
    vec3 m_MeshSize;
    QVector<Vertex> m_Vertices;
    QVector<uint32_t> m_Indices;

    MeshType m_Type;
    QString m_Path;
};

inline QJsonObject Mesh::save() const
{
    QJsonObject mesh;

    mesh["type"] = m_Type;
    mesh["path"] = m_Path;

    return mesh;
}

inline void Mesh::load(const QJsonObject& data)
{
    if (data.contains("path") && data.contains("type"))
    {
        m_Path = data["path"].toString();
        m_Type = (MeshType)(data["type"].toInt() < MeshType::MAX? data["type"].toInt() : 0);

        Load(m_Type);
    }
}

inline Mesh::MeshType Mesh::type() const
{
    return m_Type;
}

inline bool Mesh::valid() const
{
    return m_Type < MeshType::MAX;
}

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
    case MeshType::CUSTOM: return "Custom";
    default:               return "";
    }
}
