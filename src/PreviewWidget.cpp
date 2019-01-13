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

using cc::util::max;
using cc::math::radians;
using cc::math::perspective;
using cc::math::lookAt;

PreviewWidget::PreviewWidget(QWidget* parent)
    : QGLWidget(parent)
    , m_CameraPos(.0f, .0f, 2.f)
    , m_Initialized(false)
    , m_Wireframe(false)
{
}

PreviewWidget::PreviewWidget(QGLFormat format, QWidget* parent)
    : QGLWidget(format, parent)
    , m_CameraPos(.0f, .0f, 2.f)
    , m_Initialized(false)
    , m_Wireframe(false)
{
}

PreviewWidget::~PreviewWidget()
{
    delete m_Mesh;
    m_Mesh = nullptr;
}

void PreviewWidget::updateCamera()
{
    vec3 eye = m_CameraPos;
    vec3 center(.0f, .0f, .0f);
    vec3 up(.0f, 1.f, .0f);

    m_ModelView = lookAt(eye, center, up);
}

void PreviewWidget::initializeGL()
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    vec4 light_position{1., 1., 1., .0};
    glLightfv(GL_LIGHT0, GL_POSITION, reinterpret_cast<float*>(&light_position));

    glEnable(GL_COLOR_MATERIAL);

    glewExperimental = true;
    m_Initialized = (glewInit() == GLEW_OK);

    m_Mesh = new Mesh();
    m_Mesh->Load(Mesh::SPHERE);

    updateCamera();
}

void PreviewWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);

    mat4 projection = perspective(radians(45.0f), (GLfloat)width / (GLfloat)max(1, height), 0.1f, 1000.f);
    glLoadMatrixf(reinterpret_cast<float*>(&projection));
}

void PreviewWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (m_Mesh)
    {
        glPolygonMode(GL_FRONT_AND_BACK, m_Wireframe ? GL_LINE : GL_FILL);

        glMatrixMode(GL_MODELVIEW);

        vec3 tocenter = -m_Mesh->center();
        float normalizefactor = 1.f / max(m_Mesh->size().x, max(m_Mesh->size().y, m_Mesh->size().z));

        glLoadMatrixf(reinterpret_cast<float*>(&m_ModelView));
        glRotatef(m_delta.y(), 1.0f, 0.0f, 0.0f);
        glRotatef(m_delta.x(), 0.0f, 1.0f, 0.0f);
        glScalef(normalizefactor, normalizefactor, normalizefactor);
        glTranslatef(tocenter.x, tocenter.y, tocenter.z);

        m_Mesh->Draw();
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
        m_delta += (pos - m_pos);
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
    m_CameraPos.z -= (m_CameraPos.z * e->delta() / 120.0f) / 20.0f;
    updateCamera();
    updateGL();
}
