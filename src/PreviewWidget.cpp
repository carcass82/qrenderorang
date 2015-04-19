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

#include "PreviewWidget.h"

PreviewWidget::PreviewWidget(QWidget* parent)
    : QGLWidget(parent)
    , m_alpha(0.0)
    , m_beta(0.0)
    , m_x(0.0)
    , m_y(0.0)
    , m_z(5.0)
    , m_Shader(nullptr)
    , m_Initialized(false)
{
}

PreviewWidget::~PreviewWidget()
{
    for (Mesh* m : m_Meshes)
    {
        delete m;
        m = nullptr;
    }
    m_Meshes.clear();

    if (m_Shader)
    {
        delete m_Shader;
        m_Shader = nullptr;
    }
}

void PreviewWidget::initializeGL()
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glewExperimental = GL_TRUE;
    m_Initialized = (glewInit() == GLEW_OK);

    Mesh* quad = new Mesh();
    quad->Load("/random/path");
    m_Meshes.append(quad);
}

void PreviewWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);

    //gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.01f, 1000.0f);
    height = std::max(1, height);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)width / (GLfloat)height, 0.1f, 1000.f);
    glLoadMatrixf(glm::value_ptr(projection));

    glMatrixMode(GL_MODELVIEW);
}

void PreviewWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    //gluLookAt(m_x, m_y, m_z, m_x, m_y, m_z - 1, 0, 1, 0);
    glm::vec3 eye(m_x, m_y, m_z);
    glm::vec3 center(m_x, m_y, m_z - 1);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::mat4 mv = glm::lookAt(eye, center, up);
    glLoadMatrixf(glm::value_ptr(mv));

    glRotatef(m_beta, 1, 0, 0);
    glRotatef(m_alpha, 0, 1, 0);

    if (m_Shader != NULL && m_Shader->isUsable())
    {
        for (int i = 0; i < m_Shader->passCount(); ++i)
        {
            m_Shader->render(i);
            m_Meshes.at(0)->Draw();
            m_Shader->disable();
        }
    }
}

void PreviewWidget::mousePressEvent(QMouseEvent* e)
{
    m_pos = e->pos();
    m_button = e->button();
}

void PreviewWidget::mouseMoveEvent(QMouseEvent* e)
{
    QPoint pos = e->pos();
    if (m_button == Qt::LeftButton)
    {
        m_alpha += (240.0f * (pos - m_pos).x()) / height();
        m_beta += (240.0f * (pos - m_pos).y()) / height();

        if (m_beta < -90)
            m_beta = -90;
        else if (m_beta > 90)
            m_beta = 90;
    }

    m_pos = pos;
    updateGL();
}

void PreviewWidget::mouseReleaseEvent(QMouseEvent* e)
{
    m_pos = e->pos();
    m_button = Qt::NoButton;
}

void PreviewWidget::wheelEvent(QWheelEvent* e)
{
    m_z += (m_z * e->delta() / 120.0f) / 20.0f;
    updateGL();
}
