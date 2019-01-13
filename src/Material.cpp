#include "Material.h"

Material::Material()
{
    m_Props.ambient =  vec4{ 0.1f,  0.1f,  0.1f,  1.f };
    m_Props.diffuse =  vec4{ 0.2f,  0.3f,  0.9f,  1.f };
    m_Props.specular = vec4{ 0.9f,  0.9f,  0.9f,  1.f };
    m_Props.emission = vec4{  .0f,   .0f,   .0f,  1.f };
    m_Props.shininess = 12;
}

Material::~Material()
{
    for (auto Shader : m_Shaders)
    {
        delete Shader;
    }
    m_Shaders.clear();
}

void Material::BeginDraw(int pass)
{
    glMaterialfv(GL_FRONT, GL_AMBIENT,   value_ptr(m_Props.ambient));
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   value_ptr(m_Props.diffuse));
    glMaterialfv(GL_FRONT, GL_SPECULAR,  value_ptr(m_Props.specular));
    glMaterialfv(GL_FRONT, GL_EMISSION,  value_ptr(m_Props.emission));
    glMaterialf( GL_FRONT, GL_SHININESS, m_Props.shininess);

    if (pass < m_Shaders.size() && m_Shaders[pass] != nullptr)
    {
        m_currentPass = pass;
        m_Shaders[pass]->start();
    }
}

void Material::EndDraw()
{
    if (m_currentPass < m_Shaders.size() && m_Shaders[m_currentPass] != nullptr)
    {
        m_Shaders[m_currentPass]->stop();
    }
}
