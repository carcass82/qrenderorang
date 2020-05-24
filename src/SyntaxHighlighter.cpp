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
    // #TODO: these should be customizabile
    keywordsFormat.setForeground(Qt::blue);

    builtinsFormat.setForeground(Qt::magenta);

    functionFormat.setForeground(Qt::darkMagenta);
    functionFormat.setFontWeight(QFont::Bold);

    preprocessorFormat.setForeground(Qt::darkGreen);
    
    commentFormat.setForeground(Qt::darkGreen);
    commentFormat.setFontItalic(true);
    



    // flow control
    QStringList keywords{ "if", "for", "switch", "while", "break", "case", "continue", "default", "discard", "do", "else", "return" };

    // types
    keywords << QStringList{ "void", "bool", "int", "uint", "float", "double", "atomic_uint", "struct",
                             "(d|b|i|u)?vec[2-4]", "(d)?mat[2-4]", "(d)?mat2x[2-4]", "(d)?mat3x[2-4]", "(d)?mat4x[2-4]",
                             "(i|u)?(sampler|image)([1-3]D|Cube(Array|(Array)?Shadow)?|(2DRect|[1-2]D(Array)?)(Shadow)?|Buffer|2DMS(Array)?)" };

    // qualifiers
    keywords << QStringList{ "layout", "attribute", "uniform", "varying", "const", "(in|(in)?out)", "sampler",
                             "coherent", "volatile", "restrict", "readonly", "writeonly", "invariant",
                             "centroid", "patch", "flat", "noperspective", "smooth" };

    // builtin attributes
    QStringList builtins{ "gl_((Texture|ModelView|(ModelView)?Projection)Matrix(Inverse|(Inverse)?Transpose)?)",
                          "gl_((Eye|Object)Plane(Q|R|S|T)|Frag(Color|Coord|Dat|Depth))",
                          "gl_(Back|Front)Light(Model)?Product", "gl_((Back|Front)?(Secondary)?|TextureEnv)Color",
                          "gl_(Vertex(ID)?|TexCoord|InstanceID|PrimitiveIDIn|Position|Point(Coord|Parameters|Size)?|PointSize|Layer|Fog((Frag)?Coord|Parameters)?)",
                          "gl_(Light((Source|Model)(Parameters)?|(Model)?Products))",
                          "gl_(Clip(Plane|Distance|Vertex)|(Back|Front)Material|DepthRange(Parameters)?|Normal(Matrix|Scale)?)",
                          "gl_(FrontFacing|MaterialParameters|MultiTexCoord[0-7])" };

    // functions
    QStringList functions{ "texture([1-3]D(Lod|Proj(Lod)?)?|Cube(Lod)?)?)|(shadow[1-2]D(Lod|Proj(Lod)?)?",
                           "ceil", "clamp", "floor", "fract", "min", "max", "mix", "mod", "sign", "abs",
                           "sin", "cos", "tan", "asin", "acos", "atan", "radians", "degrees",
                           "pow", "((exp|log)(2)?)", "(inverse)?sqrt", "noise[1-4]",
                           "smoothstep", "step", "outerProduct", "ftransform", "cross", "distance", "dot",
                           "transpose", "faceforward", "length", "normalize", "reflect", "refract",
                           "dFdx", "dFdy", "fwidth", "matrixCompMult", "all", "any", "equal", "not",
                           "((greater|less)Than(Equal)?)" , "notEqual" };
    
    //
    // register all parsing rules
    //
    highlightingRules.append({ QRegExp("//[^\n]*"), commentFormat });

    highlightingRules.append({ QRegExp("^\\s*#[^\n]*"), preprocessorFormat });

    foreach(QString pattern, keywords)  { highlightingRules.append({ QRegExp("\\b" + pattern + "\\b"),      keywordsFormat }); }
    
    foreach(QString pattern, builtins)  { highlightingRules.append({ QRegExp("\\b" + pattern + "\\b"),      builtinsFormat }); }
    
    foreach(QString pattern, functions) { highlightingRules.append({ QRegExp("\\b" + pattern + "+(?=\\()"), functionFormat }); }
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

    // multiline comment management
    QRegExp commentStartExpression("/\\*");
    QRegExp commentEndExpression("\\*/");

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

        setFormat(startIndex, commentLength, commentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
