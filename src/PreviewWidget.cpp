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
#include <QPainter>
#include <QPoint>
#include <QElapsedTimer>
#include <QColorSpace>
#include "ShaderEditor.h"
#include "Mesh.h"

PreviewWidget::PreviewWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    QSurfaceFormat glFormat;
    glFormat.setVersion(3, 3);
    glFormat.setProfile(QSurfaceFormat::CompatibilityProfile);
    glFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    glFormat.setColorSpace(QColorSpace::SRgb);
    glFormat.setSamples(4);

    setFormat(glFormat);
    setTextureFormat(GL_SRGB_ALPHA);
}

void PreviewWidget::resetTransforms()
{
    m_CameraDistance = 2.f;
    m_delta = QPoint();
}

void PreviewWidget::updateMatrices()
{
    m_CameraDistance = max(1.f, m_CameraDistance);
    m_delta.setY(clamp(m_delta.y(), -89, 89));

    float phi = 90 + m_delta.x();
    float theta = m_delta.y();
    float radius = m_CameraDistance;

    vec3 eye;
    eye.x = cosf(radians(phi)) * cosf(radians(theta));
    eye.y = sinf(radians(theta));
    eye.z = sinf(radians(phi)) * cosf(radians(theta));
    eye *= radius;
    
    vec3 center(.0f, .0f, 0.f);
    vec3 up(.0f, 1.f, .0f);

    m_ViewMatrix = lookAt(eye, center, up);

    m_ModelMatrix = mat4(1.f);
    if (m_Mesh)
    {
        vec3 tocenter = center - m_Mesh->center();
        float normalizefactor = 1.f / max(m_Mesh->size().x, max(m_Mesh->size().y, m_Mesh->size().z));
                
        m_ModelMatrix = scale(m_ModelMatrix, vec3(normalizefactor));
        m_ModelMatrix = translate(m_ModelMatrix, tocenter);
    }

    m_ModelViewMatrix = m_ViewMatrix * m_ModelMatrix;
    m_NormalMatrix = mat3(transpose(inverse(m_ModelViewMatrix)));
    m_ModelViewProjectionMatrix = m_ProjectionMatrix * m_ModelViewMatrix;


    setShaderParameter("data.camera", eye);
    setShaderParameter("data.window", vec2(width(), height()));

    setShaderParameter("matrix.model", m_ModelMatrix);
    setShaderParameter("matrix.view", m_ViewMatrix);
    setShaderParameter("matrix.projection", m_ProjectionMatrix);
    setShaderParameter("matrix.MV", m_ModelViewMatrix);
    setShaderParameter("matrix.MVP", m_ModelViewProjectionMatrix);
    setShaderParameter("matrix.normal", m_NormalMatrix);
}

void PreviewWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_FRAMEBUFFER_SRGB);

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_LIGHTING);

    resetTransforms();
    updateMatrices();

    QString unlitVS = R"vs(#version 330
                      layout (location = 0) in vec3 position;
                      uniform struct { mat4 MVP; } matrix;
                      void main() { gl_Position = matrix.MVP * vec4(position, 1.0); })vs";

    QString unlitFS = R"fs(#version 330
                      out vec4 outColor;
                      void main() { outColor = vec4(1,1,1,1); })fs";

    QString skyVS = R"vs(#version 330
                         uniform struct { mat4 view; mat4 projection; } matrix;
                         out vec3 texCoords;
                         void main()
                         {
                             const vec3 vertices[3] = vec3[3](vec3(-1,-1,0), vec3(3,-1,0), vec3(-1,3,0));
                             vec3 position = vertices[gl_VertexID];

                             texCoords = inverse(mat3(matrix.view)) * (inverse(matrix.projection) * vec4(position,1)).xyz;
                             gl_Position = vec4(position.xy,1,1);
                         })vs";

    QString skyFS = R"fs(#version 330
                      out vec4 outColor;
                      in vec3 texCoords;
                      uniform sampler2D sky;

                      const float PI = 3.1415926538;
                      const float PI2 = 2 * PI;
                      vec2 SampleSphericalMap(vec3 v)
                      {
                          return vec2(atan(v.z, v.x) / PI2, asin(v.y) / PI) + 0.5;
                      }                      

                      void main()
                      {
                          vec2 uv = SampleSphericalMap(normalize(texCoords));
                          outColor = vec4(texture(sky, vec2(uv.x, 1 - uv.y)).rgb, 1);
                      })fs";

    QString log;
    if (!buildShader(unlitVS, unlitFS, m_UnlitSP, log))
    {
        LOG(QString("Unlit Shader compilation FAILED: %1").arg(log));
    }

    if (!buildShader(skyVS, skyFS, m_SkySP, log))
    {
        LOG(QString("Sky Shader compilation FAILED: %1").arg(log));
    }

    // create a simple blue-ish gradient as default sky
    constexpr int steps = 16;
    QByteArray defaultSky;
    for (int i = 0; i < steps; ++i)
    {
        defaultSky += (char)lerp( 5,  0, i / (float)steps);
        defaultSky += (char)lerp( 5,  0, i / (float)steps);
        defaultSky += (char)lerp(50, 20, i / (float)steps);
        defaultSky += (char)255;
    }
    defaultSkyTexture.load((uint8_t*)defaultSky.constData(), defaultSky.size(), vec3{ 1, steps, 1 });
    setShaderResource("sky", defaultSkyTexture, false);

    m_Initialized = true;
    LOG(QString("OpenGL initialized: %1 (%2)").arg((const char*)glGetString(GL_VERSION), (const char*)glGetString(GL_RENDERER)));
}

void PreviewWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    m_ProjectionMatrix = perspective(radians(45.0f), (GLfloat)width / (GLfloat)max(1, height), 0.1f, 1000.f);
    glLoadMatrixf(value_ptr(m_ProjectionMatrix));
    glMatrixMode(GL_MODELVIEW);

    updateMatrices();
}

void PreviewWidget::paintGL()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    updateMesh();
    updateShader();
    updateResources();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(value_ptr(m_ModelViewMatrix));

    glPolygonMode(GL_FRONT_AND_BACK, ((m_Wireframe)? GL_LINE : GL_FILL));
    
    bindResources();

    if (m_Mesh)
    {
        activateShader((m_Unlit? m_UnlitSP : m_SP));
        updateMaterialParameters(m_Unlit ? m_UnlitSP : m_SP);
        drawMesh();
        deactivateShader();
    }

    drawSky();

    resetUpdateMaterialFlag();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    renderText({ 0, 0 },
               QString("%1 %2 %3").arg(m_SP != 0? "" : "[Invalid Shader]", m_Wireframe ? "Wireframe" : "Fill", m_Unlit ? "Unlit" : "Lit"),
               { 255, 255, 0, 255 });
}

void PreviewWidget::renderText(const vec2& textPos, const QString& str, const vec4& color, const QFont& font)
{
    QFontMetrics fontMetrics(font);
    vec2 textSize(fontMetrics.horizontalAdvance(str), fontMetrics.height());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QColor(color.r, color.g, color.b, color.a));
    painter.setFont(font);
    painter.drawText(textPos.x, textSize.y + textPos.y, str);
    painter.end();
}

void PreviewWidget::updateMesh()
{
    if (m_newMesh && m_newMesh->valid())
    {
        delete m_Mesh;
        m_Mesh = std::exchange(m_newMesh, nullptr);

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLuint vb;
        glGenBuffers(1, &vb);
        glBindBuffer(GL_ARRAY_BUFFER, vb);
        glBufferData(GL_ARRAY_BUFFER, m_Mesh->numVertices() * sizeof(Vertex), m_Mesh->vertices(), GL_STATIC_DRAW);

        GLuint ib;
        glGenBuffers(1, &ib);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Mesh->numIndices() * sizeof(uint32_t), m_Mesh->indices(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, pos));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv0));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, bitangent));
        glEnableVertexAttribArray(4);

        glBindVertexArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        // #TODO: check if buffers can be "deleted" here or not
        //glDeleteBuffers(1, &vb);
        //glDeleteBuffers(1, &ib);

        std::swap(m_VAO, vao);
        glDeleteVertexArrays(1, &vao);

        resetTransforms();
        updateMatrices();
        update();
    }
}

void PreviewWidget::updateShader()
{
    if (!m_shaderRequests.isEmpty())
    {
        auto lastRequest = m_shaderRequests.back();

        QElapsedTimer timer;
        timer.start();

        GLuint newSP;
        QString errorString;
        if (buildShader(lastRequest.vs, lastRequest.fs, newSP, errorString))
        {
            LOG(QString("Compile Shader OK in %1 msecs").arg(timer.elapsed()));
            std::swap(m_SP, newSP);
        }
        else
        {
            LOG(QString("Compile Shader FAILED in %1 msecs\n%2\n").arg(QString::number(timer.elapsed()), errorString));
        }
        glDeleteProgram(newSP);

        m_shaderRequests.clear();
    }
}

bool PreviewWidget::buildShader(const QString& vs, const QString& fs, GLuint& outSP, QString& log)
{
    const auto CheckError = [&](GLuint obj, bool isProgram, /* or shader */ QString& log)
    {
        GLint res;
        GLint infologLength = 0;

        if (isProgram)
        {
            glGetProgramiv(obj, GL_LINK_STATUS, &res);
        }
        else
        {
            glGetShaderiv(obj, GL_COMPILE_STATUS, &res);
        }

        if (res != GL_TRUE)
        {
            if (isProgram)
            {
                glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
            }
            else
            {
                glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
            }

            if (infologLength > 0)
            {
                QVector<char> infoLog(infologLength);
                if (isProgram)
                {
                    glGetProgramInfoLog(obj, infologLength, nullptr, &infoLog[0]);
                }
                else
                {
                    glGetShaderInfoLog(obj, infologLength, nullptr, &infoLog[0]);
                }
                log.append(&infoLog[0]);
            }
        }

        return (res == GL_TRUE);
    };

    GLuint VS = glCreateShader(GL_VERTEX_SHADER);
    {
        QByteArray tmpSource = vs.toLocal8Bit();
        const GLchar* Source = tmpSource.constData();
        glShaderSource(VS, 1, &Source, nullptr);
        glCompileShader(VS);
    }
    QString vsLog;
    CheckError(VS, false, vsLog);
    ErrorHighlighter<ShaderEditor>::get().vserror(vsLog);
    log += vsLog;

    GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);
    {
        QByteArray tmpSource = fs.toLocal8Bit();
        const GLchar* Source = tmpSource.constData();
        glShaderSource(FS, 1, &Source, nullptr);
        glCompileShader(FS);
    }
    QString fsLog;
    CheckError(FS, false, fsLog);
    ErrorHighlighter<ShaderEditor>::get().fserror(fsLog);
    log += fsLog;

    GLuint newSP = glCreateProgram();
    glAttachShader(newSP, VS);
    glAttachShader(newSP, FS);
    glLinkProgram(newSP);
    
    glDetachShader(newSP, VS);
    glDetachShader(newSP, FS);
    glDeleteShader(VS);
    glDeleteShader(FS);

    outSP = newSP;
    return CheckError(newSP, true, log);
}

void PreviewWidget::updateResources()
{
    for (auto& request : m_textureRequests)
    {
        if (request.remove || textureParams.contains(request.textureName))
        {
            glDeleteTextures(1, &textureParams[request.textureName]);
        }

        if (request.remove)
        {
            resourceBindings.remove(textureParams[request.textureName]);
            textureParams.remove(request.textureName);
        }
        else if (request.texture)
        {
            GLuint newTexture;
            glGenTextures(1, &newTexture);

            if (request.texture->depth() == 1)
            {
                glBindTexture(GL_TEXTURE_2D, newTexture);

                for (int i = 0; i < request.texture->getMipmapCount(); ++i)
                {
                    if (request.texture->compressed())
                    {
                        glCompressedTexImage2D(GL_TEXTURE_2D,
                                               i,
                                               request.textureSRGB? request.texture->glsRGBFormat() : request.texture->glFormat(),
                                               request.texture->width(i),
                                               request.texture->height(i),
                                               0,
                                               request.texture->pixelsSize(i),
                                               request.texture->pixels(i));
                    }
                    else
                    {
                        glTexImage2D(GL_TEXTURE_2D,
                                     i,
                                     request.textureSRGB? request.texture->glsRGBFormat() : request.texture->glFormat(),
                                     request.texture->width(i),
                                     request.texture->height(i),
                                     0,
                                     GL_RGBA,
                                     request.texture->glDataType(),
                                     request.texture->pixels(i));
                    }
                }

                glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, max(0, request.texture->getMipmapCount() - 1));

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, request.texture->getMinFilter());
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, request.texture->getMagFilter());

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, request.texture->getWrapMode());
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, request.texture->getWrapMode());
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, request.texture->getWrapMode());

                // read back as RGB for UI preview
                unsigned char* rgba = new unsigned char[request.texture->width() * request.texture->height() * 4];
                glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
                request.texture->onLoad(rgba);
                delete[] rgba;
            }

            setShaderParameter(request.textureName, newTexture);
        }

        m_uploadMaterialParams = true;
    }

    m_textureRequests.clear();
}

void PreviewWidget::bindResources()
{
    unsigned int texUnit = 0;
    for (auto it = textureParams.constBegin(); it != textureParams.constEnd(); ++it, ++texUnit)
    {
        glActiveTexture(GL_TEXTURE0 + texUnit);
        glBindTexture(GL_TEXTURE_2D, it.value());

        resourceBindings[it.value()] = texUnit;
    }
}

unsigned int PreviewWidget::resourceID(GLuint resource)
{
    return resourceBindings.contains(resource)? resourceBindings[resource] : -1;
}

void PreviewWidget::updateMaterialParameters(GLuint program)
{
    if (m_uploadMaterialParams)
    {
        for (auto it = textureParams.constBegin(); it != textureParams.constEnd(); ++it)
        {
            glUniform1i(glGetUniformLocation(program, qPrintable(it.key())), resourceID(it.value()));
        }

        for (auto it = floatParams.constBegin(); it != floatParams.constEnd(); ++it)
        {
            glUniform1f(glGetUniformLocation(program, qPrintable(it.key())), it.value());
        }

        for (auto it = vec2Params.constBegin(); it != vec2Params.constEnd(); ++it)
        {
            glUniform2fv(glGetUniformLocation(program, qPrintable(it.key())), 1, value_ptr(it.value()));
        }

        for (auto it = vec3Params.constBegin(); it != vec3Params.constEnd(); ++it)
        {
            glUniform3fv(glGetUniformLocation(program, qPrintable(it.key())), 1, value_ptr(it.value()));
        }

        for (auto it = vec4Params.constBegin(); it != vec4Params.constEnd(); ++it)
        {
            glUniform4fv(glGetUniformLocation(program, qPrintable(it.key())), 1, value_ptr(it.value()));
        }

        for (auto it = mat3Params.constBegin(); it != mat3Params.constEnd(); ++it)
        {
            glUniformMatrix3fv(glGetUniformLocation(program, qPrintable(it.key())), 1, GL_FALSE, value_ptr(it.value()));
        }

        for (auto it = mat4Params.constBegin(); it != mat4Params.constEnd(); ++it)
        {
            glUniformMatrix4fv(glGetUniformLocation(program, qPrintable(it.key())), 1, GL_FALSE, value_ptr(it.value()));
        }
    }
}

void PreviewWidget::activateShader(GLuint sp)
{
    static GLuint lastsp = sp;
    if (lastsp != sp)
    {
        m_uploadMaterialParams = true;
        lastsp = sp;
    }

    glUseProgram(sp);
}

void PreviewWidget::deactivateShader()
{
    glUseProgram(0);
}

void PreviewWidget::drawMesh()
{
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)m_Mesh->numIndices(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void PreviewWidget::drawSky()
{
    glUseProgram(m_SkySP);
    updateMaterialParameters(m_SkySP);
    
    // no need for geometry data, vertex positions are procedurally generated in VS
    // see: https://stackoverflow.com/a/59739538
    glBindVertexArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUseProgram(0);
}

//
//
//

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
    updateMatrices();
    update();
}

void PreviewWidget::mouseReleaseEvent(QMouseEvent* e)
{
    m_pos = e->pos();
    m_button = Qt::NoButton;
    updateMatrices();
    update();
}

void PreviewWidget::wheelEvent(QWheelEvent* e)
{
    m_CameraDistance -= radians(e->angleDelta().y() / 8);
    updateMatrices();
    update();
}
