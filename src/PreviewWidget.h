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

#include <GL/glew.h>

#define GLM_FORCE_RADIANS
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <QGLWidget>
#include <QMessageBox>
#include <QMouseEvent>

#include "Mesh.h"
#include "Shader.h"


class PreviewWidget : public QGLWidget
{
public:
    PreviewWidget(QWidget *parent = 0);
    ~PreviewWidget();
    bool isInitialized() { return m_Initialized; }
    void setShader(Shader *s) { m_Shader = s; }

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void wheelEvent(QWheelEvent*);

private:
    void DrawSkybox(uint32_t size, const glm::vec3& center);

    float m_alpha;
    float m_beta;
    float m_x, m_y, m_z;
    Qt::MouseButton m_button;
    QPoint m_pos;
    Shader* m_Shader;
    QList<Mesh*> m_Meshes;
    bool m_Initialized;
};
