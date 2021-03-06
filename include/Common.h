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
#include <QPlainTextEdit>
#include <QDateTime>
#include <QDebug>
#include <QRegularExpression>

#include "cclib/cclib.h"
using cc::math::PI;
using cc::math::vec2;
using cc::math::vec3;
using cc::math::vec4;
using cc::math::mat3;
using cc::math::mat4;
using cc::math::pmin;
using cc::math::pmax;
using cc::math::max;
using cc::math::min;
using cc::math::clamp;
using cc::math::lerp;
using cc::math::radians;
using cc::math::translate;
using cc::math::rotate;
using cc::math::scale;
using cc::math::perspective;
using cc::math::lookAt;
using cc::math::value_ptr;
using cc::gfx::srgb;
using cc::gfx::linear;
using cc::yuv::yuv;

//
//
//

struct vertex_t
{
    vec3 pos;
    vec3 normal;
    vec2 uv0;
    vec3 tangent;
    vec3 bitangent;
};

using Vertex = vertex_t;
using Index = uint32_t;

//
//
//

class Logger
{
public:
    static Logger& get()                          { static Logger instance; return instance; }
    void setOutputWidget(QPlainTextEdit* widget)  { textWidget = widget; }

    void log(const QString& message)
    {
        QString timeStamp = QDateTime::currentDateTime().toString("[hh:mm:ss.zzz]");

        if (Q_LIKELY(textWidget))
        {
            textWidget->appendPlainText(QString("%1 %2").arg(timeStamp, message));
        }

        qDebug().noquote() << "LOG:" << timeStamp << message;
    }

private:
    QPlainTextEdit* textWidget;
};

#define LOG(x) Logger::get().log(x)

//
//
//

template<typename T>
class ErrorHighlighter
{
public:
    static ErrorHighlighter& get()    { static ErrorHighlighter instance; return instance; }
    void setOutputWidgetVS(T* widget) { outputVS = widget; }
    void setOutputWidgetFS(T* widget) { outputFS = widget; }

    QSet<int> error(const QString& log)
    {
        static const QRegularExpression lineNumberFinder(R"re(0\((\d+)\) : error)re");

        QSet<int> foundLines;

        auto match = lineNumberFinder.globalMatch(log);
        while (match.hasNext())
        {
            foundLines.insert(match.next().captured(1).toInt());
        }

        return foundLines;
    }

    void vserror(const QString& log)
    {
        if (Q_LIKELY(outputVS))
        {
            outputVS->highlightLine(error(log));
        }
    }

    void fserror(const QString& log)
    {
        if (Q_LIKELY(outputFS))
        {
            outputFS->highlightLine(error(log));
        }
    }

private:
    T* outputVS;
    T* outputFS;
};
