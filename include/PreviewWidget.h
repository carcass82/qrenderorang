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
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Compatibility>
#include <QMouseEvent>
#include "Common.h"
#include "Uniform.h"

class Mesh;
class Material;

class PreviewWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Compatibility
{
public:
    PreviewWidget(QSurfaceFormat format, QWidget *parent = 0);
    bool isInitialized() const;
    bool wireframe() const;
    bool toggleWireframe();
    bool toggleUnlit();
    bool unlit() const;
    Mesh* mesh() const;
    void setMesh(Mesh* mesh);

    void setShader(const QString& v, const QString& f);

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;

protected:
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseMoveEvent(QMouseEvent*) override;
    virtual void mouseReleaseEvent(QMouseEvent*) override;
    virtual void wheelEvent(QWheelEvent*) override;

private:
    void renderText(const vec2& textPos, const QString& text, const vec4& color = vec4(255), const QFont& font = QFontDatabase::systemFont(QFontDatabase::FixedFont));
    void setupMaterial();
    void drawMesh();
    void updateMesh();
    void updateMaterial();
    void updateShader();
    void resetTransforms();
    void updateMatrices();

    vec3 m_CameraPos;
    Qt::MouseButton m_button;
    QPoint m_pos;
    QPoint m_delta;
    
    mat4 m_ViewMatrix;
    mat4 m_ModelMatrix;
    mat4 m_ProjectionMatrix;
    
    bool m_Initialized = false;
    bool m_Wireframe = false;
    bool m_Unlit = false;
    
    Mesh* m_Mesh = nullptr;
    Mesh* m_newMesh = nullptr;
    GLuint m_VAO;

    bool m_uploadMaterialParams = false;
    QVector<Uniform> Uniforms;

    bool m_buildShader = false;
    QString m_VS;
    QString m_FS;
    GLuint m_SP = 0;
};


inline bool PreviewWidget::isInitialized() const
{
    return m_Initialized;
}

inline Mesh* PreviewWidget::mesh() const
{
    return m_Mesh;
}

inline void PreviewWidget::setMesh(Mesh* mesh)
{
    m_newMesh = mesh;
    update();
}

inline void PreviewWidget::setShader(const QString& v, const QString& f)
{
    m_VS = v;
    m_FS = f;
    m_buildShader = true;
    update();
}

inline bool PreviewWidget::toggleWireframe()
{
    m_Wireframe = !m_Wireframe;
    update();

    return m_Wireframe;
}

inline bool PreviewWidget::wireframe() const
{
    return m_Wireframe;
}

inline bool PreviewWidget::toggleUnlit()
{
    m_Unlit = !m_Unlit;
    update();

    return m_Unlit;
}

inline bool PreviewWidget::unlit() const
{
    return m_Unlit;
}
