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

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobj/tiny_obj_loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
using glm::vec3;
using glm::vec2;

Mesh::Mesh()
    : m_Material(nullptr)
{
}

Mesh::~Mesh()
{
    delete m_Material;
    m_Material = nullptr;
}

void Mesh::Draw()
{
    glColor3f(1.0f, 1.0f, 1.0f);

    if (m_Material == nullptr)
    {
        m_Material = new Material(); // load default material
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_VBuffer);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (GLvoid*)offsetof(Vertex, pos));

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

    glClientActiveTexture(GL_TEXTURE0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv0));

    glClientActiveTexture(GL_TEXTURE1);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_FLOAT, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tangent));
    
    glClientActiveTexture(GL_TEXTURE2);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_FLOAT, sizeof(Vertex), (GLvoid*)offsetof(Vertex, bitangent));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBuffer);

    m_Material->BeginDraw(0);

    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, (GLvoid*)0);

    m_Material->EndDraw();

    glClientActiveTexture(GL_TEXTURE2);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE1);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE0);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void Mesh::Load(MeshType Shape)
{
    m_Vertices.clear();
    m_Indices.clear();

    switch (Shape)
    {
    case CUBE:
    {
        const vec3 front[] = {
            { -1.0f, -1.0f, 1.0f },
            { 1.0f, -1.0f, 1.0f },
            { 1.0f,  1.0f, 1.0f },
            { -1.0f, 1.0f, 1.0f }
        };

        const vec3 back[] = {
            { -1.0f, -1.0f, -1.0f },
            { -1.0f,  1.0f, -1.0f },
            { 1.0f, 1.0f, -1.0f },
            { 1.0f, -1.0f, -1.0f }
        };

        const vec2 uv[] = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f }
        };

        const vec3 normals[] = {
            { 0.0f, 0.0f, 1.0f },    // front
            { 0.0f, 0.0f, -1.0f },   // back
            { 0.0f, 1.0f, 0.0f },    // top
            { 0.0f, -1.0f, 0.0f },   // bottom
            { 1.0f, 0.0f, 0.0f },    // right
            { -1.0f, 0.0f, 0.0f }    // left
        };
        
        // front
        m_Vertices.push_back({ front[0], normals[0], uv[0], vec3(.0), vec3(.0) }); // 0
        m_Vertices.push_back({ front[1], normals[0], uv[1], vec3(.0), vec3(.0) }); // 1
        m_Vertices.push_back({ front[2], normals[0], uv[2], vec3(.0), vec3(.0) }); // 2
        m_Vertices.push_back({ front[3], normals[0], uv[3], vec3(.0), vec3(.0) }); // 3
        m_Indices.push_back(0); m_Indices.push_back(1); m_Indices.push_back(3);
        m_Indices.push_back(3); m_Indices.push_back(1); m_Indices.push_back(2);

        // back
        m_Vertices.push_back({ back[0], normals[1], uv[1], vec3(.0), vec3(.0) }); // 4
        m_Vertices.push_back({ back[1], normals[1], uv[2], vec3(.0), vec3(.0) }); // 5
        m_Vertices.push_back({ back[2], normals[1], uv[3], vec3(.0), vec3(.0) }); // 6
        m_Vertices.push_back({ back[3], normals[1], uv[0], vec3(.0), vec3(.0) }); // 7
        m_Indices.push_back(4); m_Indices.push_back(7); m_Indices.push_back(5);
        m_Indices.push_back(5); m_Indices.push_back(7); m_Indices.push_back(6);

        // top
        m_Vertices.push_back({ back[1],  normals[2], uv[3], vec3(.0), vec3(.0) }); // 8
        m_Vertices.push_back({ front[3], normals[2], uv[0], vec3(.0), vec3(.0) }); // 9
        m_Vertices.push_back({ front[2], normals[2], uv[1], vec3(.0), vec3(.0) }); // 10
        m_Vertices.push_back({ back[2],  normals[2], uv[2], vec3(.0), vec3(.0) }); // 11
        m_Indices.push_back(8); m_Indices.push_back(9); m_Indices.push_back(10);
        m_Indices.push_back(10); m_Indices.push_back(11); m_Indices.push_back(8);

        // bottom
        m_Vertices.push_back({ back[0],  normals[3], uv[3], vec3(.0), vec3(.0) }); // 12
        m_Vertices.push_back({ front[0], normals[3], uv[2], vec3(.0), vec3(.0) }); // 13
        m_Vertices.push_back({ front[1], normals[3], uv[0], vec3(.0), vec3(.0) }); // 14
        m_Vertices.push_back({ back[3],  normals[3], uv[1], vec3(.0), vec3(.0) }); // 15
        m_Indices.push_back(12); m_Indices.push_back(13); m_Indices.push_back(14);
        m_Indices.push_back(14); m_Indices.push_back(15); m_Indices.push_back(12);

        // right
        m_Vertices.push_back({ back[2],  normals[4], uv[1], vec3(.0), vec3(.0) }); // 16
        m_Vertices.push_back({ front[1], normals[4], uv[2], vec3(.0), vec3(.0) }); // 17
        m_Vertices.push_back({ front[2], normals[4], uv[3], vec3(.0), vec3(.0) }); // 18
        m_Vertices.push_back({ back[3],  normals[4], uv[0], vec3(.0), vec3(.0) }); // 19
        m_Indices.push_back(17); m_Indices.push_back(19); m_Indices.push_back(18);
        m_Indices.push_back(18); m_Indices.push_back(16); m_Indices.push_back(19);

        // left
        m_Vertices.push_back({ back[0],  normals[5], uv[0], vec3(.0), vec3(.0) }); // 20
        m_Vertices.push_back({ front[0], normals[5], uv[1], vec3(.0), vec3(.0) }); // 21
        m_Vertices.push_back({ front[3], normals[5], uv[2], vec3(.0), vec3(.0) }); // 22
        m_Vertices.push_back({ back[1],  normals[5], uv[3], vec3(.0), vec3(.0) }); // 23
        m_Indices.push_back(21); m_Indices.push_back(20); m_Indices.push_back(23);
        m_Indices.push_back(23); m_Indices.push_back(22); m_Indices.push_back(21);

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
            float theta1 = delta_theta1 * glm::pi<float>();
            float theta2 = delta_theta2 * glm::pi<float>();

            for (uint32_t lat = 0; lat < latitude; ++lat)
            {
                float delta_phi1 = (float)lat / latitude;
                float delta_phi2 = (float)(lat + 1) / latitude;
                float phi1 = delta_phi1 * 2.f * glm::pi<float>();
                float phi2 = delta_phi2 * 2.f * glm::pi<float>();

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
    }

    ComputeBoundingBox();
    ComputeTangentsAndBitangents();
    SetupBuffers();
}

void Mesh::Load(const QString& filePath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!filePath.isEmpty() && tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.toStdString().c_str()))
    {
        m_Vertices.clear();
        m_Indices.clear();

        bool recompute_normals = true;
        uint i = 0;
        for (const tinyobj::shape_t& shape : shapes)
        {
            for (const tinyobj::index_t& index : shape.mesh.indices)
            {
                const int voffset = 3 * index.vertex_index;
                vec3 pos = { attrib.vertices[voffset], attrib.vertices[voffset + 1], attrib.vertices[voffset + 2] };

                vec3 normal;
                const int noffset = 3 * index.normal_index;
                if (index.normal_index != -1)
                {
                    recompute_normals = false;
                    normal = { attrib.normals[noffset], attrib.normals[noffset + 1], attrib.normals[noffset + 2] };
                }

                vec2 uv;
                const int uvoffset = 2 * index.texcoord_index;
                if (index.texcoord_index != -1)
                {
                    uv = { attrib.texcoords[uvoffset], attrib.vertices[uvoffset + 1] };
                }

                m_Vertices.push_back({ pos, normal, uv, vec3(.0), vec3(.0) });
                m_Indices.push_back(i++);
            }
        }

        if (recompute_normals)
        {
            ComputeNormals();
        }

        ComputeBoundingBox();
        ComputeTangentsAndBitangents();
        SetupBuffers();
    }
}

void Mesh::ComputeNormals()
{
    for (int i = 0; i < m_Indices.size(); i += 3)
    {
        auto& v1 = m_Vertices[m_Indices[i + 0]];
        auto& v2 = m_Vertices[m_Indices[i + 1]];
        auto& v3 = m_Vertices[m_Indices[i + 2]];

        glm::vec3 normal = normalize(cross(v2.pos - v1.pos, v3.pos - v1.pos));
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
        v1.tangent *= (dot(cross(v1.normal, v1.tangent), v1.bitangent) < 0.0f) ? -1.0f : 1.0f;
        v2.tangent *= (dot(cross(v2.normal, v2.tangent), v2.bitangent) < 0.0f) ? -1.0f : 1.0f;
        v3.tangent *= (dot(cross(v3.normal, v3.tangent), v3.bitangent) < 0.0f) ? -1.0f : 1.0f;
    }
}

void Mesh::ComputeBoundingBox()
{
    vec3 vmin;
    vec3 vmax;
    for (auto vertex : m_Vertices)
    {
        vmin = min(vmin, vertex.pos);
        vmax = max(vmax, vertex.pos);
    }

    m_MeshCenter = vec3((vmax.x + vmin.x) / 2.f, (vmax.y + vmin.y) / 2.f, (vmax.z + vmin.z) / 2.f);
    m_MeshSize = vec3(vmax.x - vmin.x, vmax.y - vmin.y, vmax.z - vmin.z);
}

void Mesh::SetupBuffers()
{
    glGenBuffers(1, &m_VBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_IBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(uint32_t), &m_Indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
