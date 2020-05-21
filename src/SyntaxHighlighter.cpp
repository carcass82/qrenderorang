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
#include "SyntaxHighlighter.h"

GLSLSyntaxHlighter::GLSLSyntaxHlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    kwordsFormat.setForeground(Qt::blue);
    QStringList keywords;
    keywords << "uniform" << "int" << "float" << "bool" << "inout"
             << "vec2" << "vec3" << "vec4" << "ivec2" << "out"
             << "ivec3" << "ivec4" << "bvec2" << "bvec3" << "in"
             << "bvec4" << "mat2" << "mat3" << "mat4" << "attribute"
             << "sampler1D" << "sampler2D" << "sampler3D"
             << "samplerCube" << "sampler1DShadow" << "void"
             << "sampler2DShadow" << "varying" << "const";

    foreach(QString pattern, keywords)
    {
        rule.pattern = QRegExp("\\b" + pattern + "\\b");
        rule.format = kwordsFormat;
        highlightingRules.append(rule);
    }

    builtinsFormat.setForeground(Qt::magenta);
    QStringList builtins;
    builtins << "gl_Position" << "gl_Vertex" << "gl_Normal" << "gl_Color" << "gl_TexCoord"
             << "gl_Clip(Size|Vertex)" << "gl_MultiTexCoord[0-7]"
             << "gl_(Front|Back|Frag|Secondary)Color" << "gl_(Front|Back)SecondaryColor"
             << "gl_Fog" << "gl_(Fog|Point)Coord" << "gl_FragCoord" << "gl_FogFragCoord"
             << "gl_Frag(Data|Depth)" << "gl_FrontFacing"
             << "gl_ModelViewMatrix" << "gl_ProjectionMatrix"
             << "gl_ModelViewProjectionMatrix" << "gl_ModelViewMatrixInverse"
             << "gl_ModelViewProjectionMatrixInverse" << "gl_NormalMatrix"
             << "gl_NormalScale" << "gl_DepthRange" << "gl_Point"
             << "gl_ModelViewMatrixInverseTranspose" << "gl_LightSource"
             << "gl_ModelViewMatrixTranspose" << "gl_ProjectionMatrixTranspose"
             << "gl_ModelViewProjectionMatrixInverseTranspose" 
             << "gl_ClipPlane" << "gl_(Eye|Object)Plane[STRQ]"
             << "gl_(Front|Back)Material" << "gl_(Front|Back)LightProduct";

    foreach(QString pattern, builtins)
    {
        rule.pattern = QRegExp("\\b" + pattern + "\\b");
        rule.format = builtinsFormat;
        highlightingRules.append(rule);
    }

    functionFormat.setForeground(QColor::fromRgb(255, 0, 141));
    QStringList functions;
    functions << "sin" << "cos" << "tan" << "asin" << "acos" << "atan"
              << "radians" << "degrees" << "pow" << "exp" << "log"
              << "expr2" << "log2" << "sqrt" << "inversesqrt" << "abs"
              << "ceil" << "clamp" << "floor" << "fract" << "min" << "mix"
              << "max" << "mod" << "sign" << "smoothstep" << "step"
              << "ftransform" << "cross" << "distance" << "dot"
              << "faceforward" << "length" << "normalize" << "reflect"
              << "dFdx" << "dFdy" << "fwidth" << "matrixCompMult" << "all"
              << "any" << "equal" << "greaterThan" << "lessThan" << "notEqual"
              << "texture1DProj" << "texture2DProj" << "texture3DProj"
              << "textureCube" << "noise4" << "texture3D" << "not" << "noise3"
              << "texture1D" << "texture2D" << "noise1" << "noise2";

    foreach(QString pattern, functions)
    {
        rule.pattern = QRegExp("\\b" + pattern + "+(?=\\()");
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }

    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat = singleLineCommentFormat;
    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");

    preprocessorFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("^\\s*#[^\n]*");
    rule.format = preprocessorFormat;
    highlightingRules.append(rule);
}

void GLSLSyntaxHlighter::highlightBlock(const QString &text)
{
    foreach(HighlightingRule rule, highlightingRules)
    {
        QRegExp expression(rule.pattern);
        int index = text.indexOf(expression);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = text.indexOf(expression, index + length);
        }
    }

    /* multiline comment management */
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
    {
        startIndex = text.indexOf(commentStartExpression);
    }

    while (startIndex >= 0)
    {
        int endIndex = text.indexOf(commentEndExpression, startIndex);
        int commentLength;

        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
        }

        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
