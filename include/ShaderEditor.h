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
#include "Common.h"
#include <QSet>
#include <QPlainTextEdit>

//
// code mostly from Qt CodeEditor example: https://doc.qt.io/qt-5/qtwidgets-widgets-codeeditor-example.html
//
class ShaderEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    ShaderEditor(QWidget* parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent* event);
    int lineNumberAreaWidth();

    void highlightLine(const QSet<int>& lineNumbers, const QColor& highlightColor = QColor(255, 85, 85));

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect& rect, int dy);

private:

    class LineNumberArea : public QWidget
    {
    public:
        LineNumberArea(ShaderEditor* editor = nullptr)
            : QWidget(editor), codeEditor(editor)     {}

        QSize sizeHint() const override               { return QSize(codeEditor->lineNumberAreaWidth(), 0); }
        void paintEvent(QPaintEvent* event) override  { codeEditor->lineNumberAreaPaintEvent(event); }

    private:
        ShaderEditor* codeEditor;
    };

    LineNumberArea* lineNumberArea;
};
