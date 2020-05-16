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
#include "Mesh.h"

PreviewWidget::PreviewWidget(QSurfaceFormat format, QWidget* parent)
    : QOpenGLWidget(parent)
{
    setFormat(format);
}

void PreviewWidget::resetTransforms()
{
    m_CameraPos = vec3(.0f, .0f, 2.f);
    m_delta = QPoint();
}

void PreviewWidget::updateMatrices()
{
    vec3 eye = m_CameraPos;
    vec3 center(.0f, .0f, .0f);
    vec3 up(.0f, 1.f, .0f);

    m_ViewMatrix = lookAt(eye, center, up);

    if (m_Mesh)
    {
        vec3 tocenter = -(m_Mesh->center());
        float normalizefactor = 1.f / max(m_Mesh->size().x, max(m_Mesh->size().y, m_Mesh->size().z));

        m_ModelMatrix = m_ViewMatrix;
        m_ModelMatrix = rotate(m_ModelMatrix, radians(m_delta.y()), vec3{ 1.0f, 0.0f, 0.0f });
        m_ModelMatrix = rotate(m_ModelMatrix, radians(m_delta.x()), vec3{ 0.0f, 1.0f, 0.0f });
        m_ModelMatrix = scale(m_ModelMatrix, vec3(normalizefactor));
        m_ModelMatrix = translate(m_ModelMatrix, tocenter);
    }
}

void PreviewWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_LIGHTING);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    resetTransforms();
    updateMatrices();

    m_Initialized = true;
}

void PreviewWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    m_ProjectionMatrix = perspective(radians(45.0f), (GLfloat)width / (GLfloat)max(1, height), 0.1f, 1000.f);
    glLoadMatrixf(value_ptr(m_ProjectionMatrix));
    glMatrixMode(GL_MODELVIEW);
}

void PreviewWidget::paintGL()
{
    updateMesh();
    updateShader();
    updateMaterial();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);
    if (m_Mesh)
    {
        glPolygonMode(GL_FRONT_AND_BACK, m_Wireframe? GL_LINE : GL_FILL);
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(value_ptr(m_ModelMatrix));

        glUseProgram(m_Unlit? 0 : m_SP);
        setupMaterial();
        drawMesh();
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glUseProgram(0);

    renderText({ 1, 11 }, QString("%2 %3").arg(m_Wireframe ? "Wireframe" : "Fill", m_Unlit ? "Unlit" : "Lit"), { 255, 255, 0, 255 });
}

void PreviewWidget::renderText(const vec2& textPos, const QString& str, const vec4& color, const QFont& font)
{
    QPainter painter(this);
    painter.setPen(QColor(color.r, color.g, color.b, color.a));
    painter.setFont(font);
    painter.drawText(textPos.x, textPos.y, str);
    //painter.end();
}

void PreviewWidget::updateMesh()
{
    if (m_newMesh)
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
        glDeleteBuffers(1, &vb);
        glDeleteBuffers(1, &ib);

        std::swap(m_VAO, vao);
        glDeleteVertexArrays(1, &vao);

        resetTransforms();
        updateMatrices();
        update();
    }
}

void PreviewWidget::updateMaterial()
{
}

void PreviewWidget::updateShader()
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


    if (m_buildShader)
    {
        QElapsedTimer timer;
        timer.start();

        QString errorString;
        GLuint VS = glCreateShader(GL_VERTEX_SHADER);
        {
            QByteArray tmpSource = m_VS.toLocal8Bit();
            const GLchar* Source = tmpSource.constData();
            glShaderSource(VS, 1, &Source, nullptr);
            glCompileShader(VS);
        }
        CheckError(VS, false, errorString);

        GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);
        {
            QByteArray tmpSource = m_FS.toLocal8Bit();
            const GLchar* Source = tmpSource.constData();
            glShaderSource(FS, 1, &Source, nullptr);
            glCompileShader(FS);
        }
        CheckError(FS, false, errorString);

        GLuint newSP = glCreateProgram();
        glAttachShader(newSP, VS);
        glAttachShader(newSP, FS);
        glLinkProgram(newSP);

        glDetachShader(newSP, VS);
        glDetachShader(newSP, FS);
        glDeleteShader(VS);
        glDeleteShader(FS);

        QString compileLog("Compiling Shader... ");
        if (CheckError(newSP, true, errorString))
        {
            compileLog += QString("OK in %1 msecs").arg(timer.elapsed());
            std::swap(m_SP, newSP);
        }
        else
        {
            compileLog += QString("FAILED in %1 msecs\n%2\n").arg(QString::number(timer.elapsed()), errorString);
        }
        LOG(compileLog);

        glDeleteProgram(newSP);

        m_buildShader = false;
    }
}



void PreviewWidget::setupMaterial()
{
    // TODO: light should be adjustable
    const vec4 light_position{ 2.f, 2.f, -1.f, .0f };
    const vec4 white{ 1.f, 1.f, 1.f, 1.f };
    glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(light_position));
    glLightfv(GL_LIGHT0, GL_AMBIENT, value_ptr(white));
    glLightfv(GL_LIGHT0, GL_DIFFUSE, value_ptr(white));
    glLightfv(GL_LIGHT0, GL_SPECULAR, value_ptr(white));
    glEnable(GL_LIGHT0);

    const vec4 ambient{ 0.1f,  0.1f,  0.1f,  1.f };
    const vec4 diffuse{ 0.2f,  0.3f,  0.9f,  1.f };
    const vec4 specular{ 0.9f,  0.9f,  0.9f,  1.f };
    const vec4 emission{ .0f,   .0f,   .0f,  1.f };
    const float shininess = 12;
    glMaterialfv(GL_FRONT, GL_AMBIENT, value_ptr(ambient));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, value_ptr(diffuse));
    glMaterialfv(GL_FRONT, GL_SPECULAR, value_ptr(specular));
    glMaterialfv(GL_FRONT, GL_EMISSION, value_ptr(emission));
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    glEnable(GL_COLOR_MATERIAL);

    for (auto& uniform : Uniforms)
    {
        //SetUniform(uniform.value);
    }
}

void PreviewWidget::drawMesh()
{
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)m_Mesh->numIndices(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
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
    m_CameraPos.z -= (m_CameraPos.z * e->delta() / 120.0f) / 20.0f;
    updateMatrices();
    update();
}
