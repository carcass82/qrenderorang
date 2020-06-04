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
#include <QHash>
#include "Common.h"

class Mesh;
class Material;

class PreviewWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Compatibility
{
public:
    PreviewWidget(QWidget *parent = 0);
    bool isInitialized() const;
    bool wireframe() const;
    bool toggleWireframe();
    bool toggleUnlit();
    bool unlit() const;
    Mesh* mesh() const;
    void setMesh(Mesh* mesh);

    void setShader(const QString& v, const QString& f);
    
    template<typename T>
    void setShaderParameter(QHash<QString, T>& container, const QString& parameter, const T& value, bool remove);
    void setShaderParameter(const QString& parameter, GLuint value, bool remove = false);
    void setShaderParameter(const QString& parameter, float value, bool remove = false);
    void setShaderParameter(const QString& parameter, const vec2& value, bool remove = false);
    void setShaderParameter(const QString& parameter, const vec3& value, bool remove = false);
    void setShaderParameter(const QString& parameter, const vec4& value, bool remove = false);
    void setShaderParameter(const QString& parameter, const mat3& value, bool remove = false);
    void setShaderParameter(const QString& parameter, const mat4& value, bool remove = false);
    
    void setShaderResource(const QString& parameter, const char* pixels, int width, int height, int depth = 1, int bpp = 4, bool sRGB = true, bool HDR = false);
    void deleteShaderResource(const QString& parameter);

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
    unsigned int resourceID(GLuint resource);
    void updateMaterialParameters(GLuint program);
    void resetUpdateMaterialFlag();
    void activateShader(GLuint sp);
    void deactivateShader();
    void drawMesh();
    void updateMesh();
    void drawSky();
    void updateShader();
    void updateResources();
    void bindResources();
    bool buildShader(const QString& vs, const QString& fs, GLuint& outSP, QString& log);
    void resetTransforms();
    void updateMatrices();

    float m_CameraDistance;
    Qt::MouseButton m_button;
    QPoint m_pos;
    QPoint m_delta;
    
    mat4 m_ViewMatrix;
    mat4 m_ModelMatrix;
    mat4 m_ModelViewMatrix;
    mat4 m_ProjectionMatrix;
    mat4 m_ModelViewProjectionMatrix;
    mat3 m_NormalMatrix;
    
    bool m_Initialized = false;
    bool m_Wireframe = false;
    bool m_Unlit = false;
    
    Mesh* m_Mesh = nullptr;
    Mesh* m_newMesh = nullptr;
    GLuint m_VAO;

    GLuint m_SP = 0;
    GLuint m_UnlitSP = 0;
    GLuint m_SkySP = 0;

    bool m_uploadMaterialParams = false;
    QHash<QString, GLuint> textureParams;
    QHash<GLuint, unsigned int> resourceBindings;
    QHash<QString, float> floatParams;
    QHash<QString, vec2> vec2Params;
    QHash<QString, vec3> vec3Params;
    QHash<QString, vec4> vec4Params;
    QHash<QString, mat3> mat3Params;
    QHash<QString, mat4> mat4Params;

    struct ShaderRequest
    {
        QString vs;
        QString fs;
    };
    QVector<ShaderRequest> m_shaderRequests;

    struct TextureRequest
    {
        QString textureName;
        vec3 textureSize;
        bool textureSRGB;
        bool textureHDR;
        QByteArray textureData;

        bool remove;
    };
    QVector<TextureRequest> m_textureRequests;
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
    ShaderRequest request;
    request.vs = v;
    request.fs = f;

    m_shaderRequests.push_back(request);
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

inline void PreviewWidget::resetUpdateMaterialFlag()
{
    m_uploadMaterialParams = false;
}

inline void PreviewWidget::setShaderResource(const QString& parameter, const char* pixels, int width, int height, int depth, int bpp, bool sRGB, bool HDR)
{
    if (!parameter.isEmpty() && pixels && width > 0 && height > 0)
    {
        TextureRequest request;
        request.textureName = parameter;
        request.textureSize.x = width;
        request.textureSize.y = height;
        request.textureSize.z = depth;
        request.textureData = QByteArray(pixels, width * height * depth * bpp * ((HDR)? sizeof(float) : sizeof(unsigned char)));
        request.textureSRGB = sRGB;
        request.textureHDR = HDR;
        request.remove = false;

        m_textureRequests.push_back(request);

        update();
    }
}

inline void PreviewWidget::deleteShaderResource(const QString& parameter)
{
    if (textureParams.contains(parameter))
    {
        m_textureRequests.push_back({ parameter, vec3(), false, false, QByteArray(), true });
        
        update();
    }
}

inline void PreviewWidget::setShaderParameter(const QString& parameter, GLuint value, bool remove)
{
    setShaderParameter(textureParams, parameter, value, remove);
}

inline void PreviewWidget::setShaderParameter(const QString& parameter, float value, bool remove)
{
    setShaderParameter(floatParams, parameter, value, remove);
}

inline void PreviewWidget::setShaderParameter(const QString& parameter, const vec2& value, bool remove)
{
    setShaderParameter(vec2Params, parameter, value, remove);
}

inline void PreviewWidget::setShaderParameter(const QString& parameter, const vec3& value, bool remove)
{
    setShaderParameter(vec3Params, parameter, value, remove);
}

inline void PreviewWidget::setShaderParameter(const QString& parameter, const vec4& value, bool remove)
{
    setShaderParameter(vec4Params, parameter, value, remove);
}

inline void PreviewWidget::setShaderParameter(const QString& parameter, const mat3& value, bool remove)
{
    setShaderParameter(mat3Params, parameter, value, remove);
}

inline void PreviewWidget::setShaderParameter(const QString& parameter, const mat4& value, bool remove)
{
    setShaderParameter(mat4Params, parameter, value, remove);
}

template<typename T>
inline void PreviewWidget::setShaderParameter(QHash<QString, T>& container, const QString& parameter, const T& value, bool remove)
{
    if (!parameter.isEmpty() && (!container.contains(parameter) || container.value(parameter) != value || remove))
    {
        if (remove)
        {
            container.remove(parameter);
        }
        else
        {
            container[parameter] = value;
        }

        m_uploadMaterialParams = true;
        update();
    }
}
