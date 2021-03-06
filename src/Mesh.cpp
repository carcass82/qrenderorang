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
#include "Mesh.h"
#include <QMap>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobj/tiny_obj_loader.h"

Mesh* Mesh::Load(MeshType Shape)
{
    m_Vertices.clear();
    m_Indices.clear();

    switch (Shape)
    {
    case CUBE:
    {
        const vec3 top(.5f, .5f, .5f);
        const vec3 bottom(-.5f, -.5f, -.5f);

        const vec3 vertices[] = {
            { top.x,    top.y,    bottom.z },
            { top.x,    bottom.y, bottom.z },
            { top.x,    top.y,    top.z    },
            { top.x,    bottom.y, top.z    },
            { bottom.x, top.y,    bottom.z },
            { bottom.x, bottom.y, bottom.z },
            { bottom.x, top.y,    top.z    },
            { bottom.x, bottom.y, top.z    }
        };

        const vec2 uv[] = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 0.0f, 1.0f },
            { 1.0f, 1.0f }
        };

        const vec3 normals[] = {
            {  0.0f,  1.0f,  0.0f },
            {  0.0f,  0.0f,  1.0f },
            { -1.0f,  0.0f,  0.0f },
            {  0.0f, -1.0f,  0.0f },
            {  1.0f,  0.0f,  0.0f },
            {  0.0f,  0.0f, -1.0f }
        };

        m_Vertices.push_back({ vertices[4], normals[0], uv[0], vec3(.0f), vec3(.0f)}); // 0
        m_Vertices.push_back({ vertices[2], normals[0], uv[3], vec3(.0f), vec3(.0f)}); // 1
        m_Vertices.push_back({ vertices[0], normals[0], uv[1], vec3(.0f), vec3(.0f)}); // 2
        m_Indices.push_back(0); m_Indices.push_back(1); m_Indices.push_back(2);

        m_Vertices.push_back({ vertices[2], normals[1], uv[1], vec3(.0f), vec3(.0f) }); // 3
        m_Vertices.push_back({ vertices[7], normals[1], uv[2], vec3(.0f), vec3(.0f) }); // 4
        m_Vertices.push_back({ vertices[3], normals[1], uv[3], vec3(.0f), vec3(.0f) }); // 5
        m_Indices.push_back(3); m_Indices.push_back(4); m_Indices.push_back(5);

        m_Vertices.push_back({ vertices[6], normals[2], uv[1], vec3(.0f), vec3(.0f) }); // 6
        m_Vertices.push_back({ vertices[5], normals[2], uv[2], vec3(.0f), vec3(.0f) }); // 7
        m_Vertices.push_back({ vertices[7], normals[2], uv[3], vec3(.0f), vec3(.0f) }); // 8
        m_Indices.push_back(6); m_Indices.push_back(7); m_Indices.push_back(8);

        m_Vertices.push_back({ vertices[1], normals[3], uv[3], vec3(.0f), vec3(.0f) }); // 9
        m_Vertices.push_back({ vertices[7], normals[3], uv[0], vec3(.0f), vec3(.0f) }); // 10
        m_Vertices.push_back({ vertices[5], normals[3], uv[2], vec3(.0f), vec3(.0f) }); // 11
        m_Indices.push_back(9); m_Indices.push_back(10); m_Indices.push_back(11);

        m_Vertices.push_back({ vertices[0], normals[4], uv[1], vec3(.0f), vec3(.0f) }); // 12
        m_Vertices.push_back({ vertices[3], normals[4], uv[2], vec3(.0f), vec3(.0f) }); // 13
        m_Vertices.push_back({ vertices[1], normals[4], uv[3], vec3(.0f), vec3(.0f) }); // 14
        m_Indices.push_back(12); m_Indices.push_back(13); m_Indices.push_back(14);

        m_Vertices.push_back({ vertices[4], normals[5], uv[1], vec3(.0f), vec3(.0f) }); // 15
        m_Vertices.push_back({ vertices[1], normals[5], uv[2], vec3(.0f), vec3(.0f) }); // 16
        m_Vertices.push_back({ vertices[5], normals[5], uv[3], vec3(.0f), vec3(.0f) }); // 17
        m_Indices.push_back(15); m_Indices.push_back(16); m_Indices.push_back(17);

        m_Vertices.push_back({ vertices[6], normals[0], uv[2], vec3(.0f), vec3(.0f) }); // 18
        m_Indices.push_back(0); m_Indices.push_back(18); m_Indices.push_back(1);

        m_Vertices.push_back({ vertices[6], normals[1], uv[0], vec3(.0f), vec3(.0f) }); // 19
        m_Indices.push_back(3); m_Indices.push_back(19); m_Indices.push_back(4);

        m_Vertices.push_back({ vertices[4], normals[2], uv[0], vec3(.0f), vec3(.0f) }); // 20
        m_Indices.push_back(6); m_Indices.push_back(20); m_Indices.push_back(7);

        m_Vertices.push_back({ vertices[3], normals[3], uv[1], vec3(.0f), vec3(.0f) }); // 21
        m_Indices.push_back(9); m_Indices.push_back(21); m_Indices.push_back(10);

        m_Vertices.push_back({ vertices[2], normals[4], uv[0], vec3(.0f), vec3(.0f) }); // 22
        m_Indices.push_back(12); m_Indices.push_back(22); m_Indices.push_back(13);

        m_Vertices.push_back({ vertices[0], normals[5], uv[0], vec3(.0f), vec3(.0f) }); // 23
        m_Indices.push_back(15); m_Indices.push_back(23); m_Indices.push_back(16);
        break;
    }

    case SPHERE:
    {
        constexpr uint32_t longitude = 64;
        constexpr uint32_t latitude = 64;
        constexpr float radius = 1.f;

        for (uint32_t lon = 0; lon < longitude; ++lon)
        {
            float delta_theta1 = (float)lon / longitude;
            float delta_theta2 = (float)(lon + 1) / longitude;
            float theta1 = delta_theta1 * PI;
            float theta2 = delta_theta2 * PI;

            for (uint32_t lat = 0; lat < latitude; ++lat)
            {
                float delta_phi1 = (float)lat / latitude;
                float delta_phi2 = (float)(lat + 1) / latitude;
                float phi1 = delta_phi1 * 2.f * PI;
                float phi2 = delta_phi2 * 2.f * PI;

                // phi2  phi1
                //  |     |
                //  2-----1 -- theta1
                //  |\    |
                //  | \   |
                //  |  \  |
                //  |   \ |
                //  3-----4 -- theta2

                //vertex1 = vertex on a sphere of radius r at spherical coords theta1, phi1
                vec3 pos = vec3{ sinf(theta1) * cosf(phi1), sinf(theta1) * sinf(phi1), cosf(theta1) };
                vec2 uv = vec2{ delta_phi1, delta_theta1 };

                Vertex v1{};
                v1.pos = pos * radius;
                v1.normal = pos;
                v1.uv0 = uv;
                m_Vertices.push_back(v1);

                //vertex2 = vertex on a sphere of radius r at spherical coords theta1, phi2
                pos = vec3{ sinf(theta1) * cosf(phi2), sinf(theta1) * sinf(phi2), cosf(theta1) };
                uv = vec2{ delta_phi2, delta_theta1 };

                Vertex v2{};
                v2.pos = pos * radius;
                v2.normal = pos;
                v2.uv0 = uv;
                m_Vertices.push_back(v2);

                //vertex3 = vertex on a sphere of radius r at spherical coords theta2, phi2
                pos = vec3{ sinf(theta2) * cosf(phi2), sinf(theta2) * sinf(phi2), cosf(theta2) };
                uv = vec2{ delta_phi2, delta_theta2 };

                Vertex v3{};
                v3.pos = pos * radius;
                v3.normal = pos;
                v3.uv0 = uv;
                m_Vertices.push_back(v3);

                //vertex4 = vertex on a sphere of radius r at spherical coords theta2, phi1
                pos = vec3{ sinf(theta2) * cosf(phi1), sinf(theta2) * sinf(phi1), cosf(theta2) };
                uv = vec2{ delta_phi1, delta_theta2 };

                Vertex v4{};
                v4.pos = pos * radius;
                v4.normal = pos;
                v4.uv0 = uv;
                m_Vertices.push_back(v4);

                uint32_t baseidx = static_cast<uint32_t>(m_Vertices.size()) - 4;

                if (lon == 0) // top cap
                {
                    m_Indices.push_back(baseidx + 0);
                    m_Indices.push_back(baseidx + 3);
                    m_Indices.push_back(baseidx + 2);
                }
                else if (lon + 1 == longitude) //end cap
                {
                    m_Indices.push_back(baseidx + 2);
                    m_Indices.push_back(baseidx + 1);
                    m_Indices.push_back(baseidx + 0);
                }
                else
                {
                    m_Indices.push_back(baseidx + 0);
                    m_Indices.push_back(baseidx + 3);
                    m_Indices.push_back(baseidx + 1);

                    m_Indices.push_back(baseidx + 1);
                    m_Indices.push_back(baseidx + 3);
                    m_Indices.push_back(baseidx + 2);
                }
            }
        }

        for (auto& v : m_Vertices)
        {
            v.normal = normalize(v.normal);
        }
        
        break;
    }

    case CUSTOM:
        return Load(m_Path);


    default:
        LOG("Mesh load failed, wrong type");
        break;
    }

    ComputeBoundingBox();
    ComputeTangentsAndBitangents();

    m_Type = Shape;

    return this;
}

Mesh* Mesh::Load(const QString& filePath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    m_Type = MAX;
    if (!filePath.isEmpty() && tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.toStdString().c_str()))
    {
        m_Type = CUSTOM;
        m_Path = filePath;

        m_Vertices.clear();
        m_Indices.clear();

        bool recompute_normals = true;
        QMap<int, int> indices_remap;
        for (const tinyobj::shape_t& shape : shapes)
        {
            for (const tinyobj::index_t& index : shape.mesh.indices)
            {
                if (indices_remap.contains(index.vertex_index))
                {
                    m_Indices.push_back(indices_remap[index.vertex_index]);
                    continue;
                }

                const int voffset = 3 * index.vertex_index;
                vec3 pos = vec3{ attrib.vertices[voffset], attrib.vertices[voffset + 1], attrib.vertices[voffset + 2] };

                vec3 normal;
                const int noffset = 3 * index.normal_index;
                if (index.normal_index != -1)
                {
                    recompute_normals = false;
                    normal = vec3{ attrib.normals[noffset], attrib.normals[noffset + 1], attrib.normals[noffset + 2] };
                }

                vec2 uv;
                const int uvoffset = 2 * index.texcoord_index;
                if (index.texcoord_index != -1)
                {
                    uv = vec2{ attrib.texcoords[uvoffset], attrib.texcoords[uvoffset + 1] };
                }

                m_Vertices.push_back({ pos, normal, uv, vec3(.0), vec3(.0) });
                m_Indices.push_back(m_Vertices.size() - 1);
                indices_remap.insert(index.vertex_index, m_Vertices.size() - 1);
            }
        }

        if (recompute_normals)
        {
            ComputeNormals();
        }

        ComputeBoundingBox();
        ComputeTangentsAndBitangents();
    }

    return this;
}

void Mesh::ComputeNormals()
{
    for (int i = 0; i < m_Indices.size(); i += 3)
    {
        auto& v1 = m_Vertices[m_Indices[i + 0]];
        auto& v2 = m_Vertices[m_Indices[i + 1]];
        auto& v3 = m_Vertices[m_Indices[i + 2]];

        vec3 normal = normalize(cross(v2.pos - v1.pos, v3.pos - v1.pos));
        v1.normal = v2.normal = v3.normal = normal;
    }
}

void Mesh::ComputeTangentsAndBitangents()
{
    // Lengyel, Eric. "Computing Tangent Space Basis Vectors for an Arbitrary Mesh"
    // http://www.terathon.com/code/tangent.html
    //
    for (int i = 0; i < m_Indices.size(); i += 3)
    {
        auto& v1 = m_Vertices[m_Indices[i + 0]];
        auto& v2 = m_Vertices[m_Indices[i + 1]];
        auto& v3 = m_Vertices[m_Indices[i + 2]];

        // Edges of the triangle : position delta
        vec3 deltaPos1 = v2.pos - v1.pos;
        vec3 deltaPos2 = v3.pos - v1.pos;

        // UV delta
        vec2 deltaUV1 = v2.uv0 - v1.uv0;
        vec2 deltaUV2 = v3.uv0 - v1.uv0;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        v1.tangent = v2.tangent = v3.tangent = tangent;
        v1.bitangent = v2.bitangent = v3.bitangent = bitangent;

        // ensure B T and N are orthogonal
        v1.tangent = normalize(tangent - v1.normal * dot(v1.normal, tangent));
        v2.tangent = normalize(tangent - v2.normal * dot(v2.normal, tangent));
        v3.tangent = normalize(tangent - v3.normal * dot(v3.normal, tangent));
        
        // account for wrong handedness
        v1.bitangent = cross(v1.normal, v1.tangent) * ((dot(cross(v1.normal, v1.tangent), v1.bitangent) < 0.0f) ? -1.0f : 1.0f);
        v2.bitangent = cross(v2.normal, v2.tangent) * ((dot(cross(v2.normal, v2.tangent), v2.bitangent) < 0.0f) ? -1.0f : 1.0f);
        v3.bitangent = cross(v3.normal, v3.tangent) * ((dot(cross(v3.normal, v3.tangent), v3.bitangent) < 0.0f) ? -1.0f : 1.0f);
    }
}

void Mesh::ComputeBoundingBox()
{
    vec3 vmin;
    vec3 vmax;
    for (auto vertex : m_Vertices)
    {
        vmin = pmin(vmin, vertex.pos);
        vmax = pmax(vmax, vertex.pos);
    }

    m_MeshCenter = vec3((vmax.x + vmin.x) / 2.f, (vmax.y + vmin.y) / 2.f, (vmax.z + vmin.z) / 2.f);
    m_MeshSize = vec3(vmax.x - vmin.x, vmax.y - vmin.y, vmax.z - vmin.z);
}
