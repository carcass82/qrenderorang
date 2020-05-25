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
#include "ShaderEditor.h"
#include "SyntaxHighlighter.h"

#include <QPainter>
#include <QTextBlock>
#include <QSCrollBar>

ShaderEditor::ShaderEditor(QWidget* parent)
	: QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, &ShaderEditor::blockCountChanged,     this, &ShaderEditor::updateLineNumberAreaWidth);
    connect(this, &ShaderEditor::updateRequest,         this, &ShaderEditor::updateLineNumberArea);
    connect(this, &ShaderEditor::cursorPositionChanged, this, &ShaderEditor::updateHighlighting);

    updateLineNumberAreaWidth(0);
}

int ShaderEditor::lineNumberAreaWidth()
{
    const int charWidth = fontMetrics().horizontalAdvance(QLatin1Char('8'));
    const int digits = QString::number(blockCount()).size();

    return charWidth * max(5, digits);
}

void ShaderEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void ShaderEditor::updateLineNumberArea(const QRect& rect, int dy)
{
    if (dy)
    {
        lineNumberArea->scroll(0, dy);
    }
    else
    {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect()))
    {
        updateLineNumberAreaWidth(0);
    }
}

void ShaderEditor::resizeEvent(QResizeEvent* e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void ShaderEditor::highlightLine(const QSet<int>& lineNumbers, const QColor& highlightColor)
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    QColor lineColor = highlightColor;
    foreach (int lineNumber, lineNumbers)
    {
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = QTextCursor(document()->findBlockByNumber(max(0, lineNumber - 1)));
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    
    setExtraSelections(extraSelections);
}

void ShaderEditor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QFont defaultFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    
    QFont boldFont = defaultFont;
    boldFont.setBold(true);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.setFont(blockNumber == textCursor().blockNumber()? boldFont : defaultFont);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignCenter, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void ShaderEditor::updateHighlighting()
{
    // #TODO: these also should be customizabile
    QTextCharFormat matchParenthesesFormat;
    matchParenthesesFormat.setBackground(Qt::lightGray);
    matchParenthesesFormat.setFontWeight(QFont::Bold);

    QTextCharFormat missingParenthesisFormat;
    missingParenthesisFormat.setForeground(Qt::red);
    matchParenthesesFormat.setFontWeight(QFont::Bold);

    QTextCharFormat selectedWordMatchFormat;
    selectedWordMatchFormat.setBackground(QColor(Qt::yellow).lighter());


    blockSignals(true); // avoid triggering recursive selectionChanged / cursorChanged
    {
        QList<QTextEdit::ExtraSelection> extraSelections;

        QTextCursor originalCursor(textCursor());
        int originalScrollPosition = verticalScrollBar()->value();

        if (auto userData = static_cast<TextBlockData*>(textCursor().block().userData()))
        {
            foreach(auto parenthesis, userData->parentheses)
            {
                int curPos = textCursor().position() - textCursor().block().position() - 1;
                if (parenthesis.position == curPos)
                {
                    if ((parenthesis.character == '(' && find(")")) ||
                        (parenthesis.character == '{' && find("}")) ||
                        (parenthesis.character == '[' && find("]")) ||
                        (parenthesis.character == ')' && find("(", QTextDocument::FindBackward)) ||
                        (parenthesis.character == '}' && find("{", QTextDocument::FindBackward)) ||
                        (parenthesis.character == ']' && find("[", QTextDocument::FindBackward)))
                    {
                        QTextEdit::ExtraSelection selection;
                        selection.format = matchParenthesesFormat;
                        selection.cursor = textCursor();
                        extraSelections.append(selection);
                    }
                    else
                    {
                        QTextEdit::ExtraSelection selection;
                        selection.format = missingParenthesisFormat;
                        selection.cursor = QTextCursor(originalCursor);
                        selection.cursor.clearSelection();
                        selection.cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                        extraSelections.append(selection);
                    }
                }
            }
        }

        QString text = textCursor().selectedText();
        if (text.size() > 1)
        {
            QTextCursor searchCursor(textCursor());
            searchCursor.movePosition(QTextCursor::Start);
            setTextCursor(searchCursor);

            while (find(text, QTextDocument::FindWholeWords))
            {
                QTextEdit::ExtraSelection selection;
                selection.format = selectedWordMatchFormat;
                selection.cursor = textCursor();
                extraSelections.append(selection);
            }
        }

        setTextCursor(originalCursor);
        verticalScrollBar()->setValue(originalScrollPosition);

        setExtraSelections(extraSelections);
    }
    blockSignals(false);
}
