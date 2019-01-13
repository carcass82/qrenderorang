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
#include "GL/glew.h"

#include <QGLWidget>
#include <QMessageBox>
#include <QMouseEvent>

#include "Mesh.h"
#include "Shader.h"

#include "cclib/cclib.h"
using cc::math::vec3;
using cc::math::mat4;


class PreviewWidget : public QGLWidget
{
public:
    PreviewWidget(QWidget *parent = 0);
    PreviewWidget(QGLFormat format, QWidget* parent = 0);
    ~PreviewWidget();
    bool isInitialized() const;
    bool wireframe() const;
    void ToggleWireframe();
    Mesh* mesh() const;
    void SetMesh(Mesh* mesh);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void wheelEvent(QWheelEvent*);

private:
    void updateCamera();

    vec3 m_CameraPos;
    Qt::MouseButton m_button;
    QPoint m_pos;
    QPoint m_delta;
    Mesh* m_Mesh;
    mat4 m_ModelView;
    bool m_Initialized;
    bool m_Wireframe;
};


inline bool PreviewWidget::isInitialized() const
{
    return m_Initialized;
}

inline Mesh* PreviewWidget::mesh() const
{
    return m_Mesh;
}

inline void PreviewWidget::SetMesh(Mesh* mesh)
{
    delete m_Mesh;
    m_Mesh = mesh;

    updateCamera();
    updateGL();
}

inline void PreviewWidget::ToggleWireframe()
{
    m_Wireframe = !m_Wireframe;
    updateGL();
}

inline bool PreviewWidget::wireframe() const
{
    return m_Wireframe;
}
