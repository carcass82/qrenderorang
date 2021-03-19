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
    QStringList functions{ "texture([1-3]D(Lod|Proj(Lod)?)?|Cube(Lod)?)?|shadow[1-2]D(Lod|Proj(Lod)?)?",
                           "ceil", "clamp", "floor", "fract", "trunc", "min", "max", "mix", "(mod(f)?)", "sign", "abs",
                           "sin", "cos", "tan", "asin", "acos", "atan", "asinh", "acosh", "atanh" "radians", "degrees", "is(inf|nan)",
                           "pow", "((exp|log)(2)?)", "(inverse)?sqrt", "noise[1-4]", "(round(Even)?)",
                           "smoothstep", "step", "outerProduct", "ftransform", "cross", "distance", "dot",
                           "transpose", "inverse", "determinant", "faceforward", "length", "normalize", "reflect", "refract",
                           "dFdx", "dFdy", "fwidth", "matrixCompMult", "all", "any", "equal", "not",
                           "((greater|less)Than(Equal)?)" , "notEqual" };
    
    //
    // register all parsing rules
    //
    foreach(QString pattern, keywords)  { highlightingRules.append({ QRegularExpression("\\b" + pattern + "\\b"), keywordsFormat }); }
    
    foreach(QString pattern, builtins)  { highlightingRules.append({ QRegularExpression("\\b" + pattern + "\\b"), builtinsFormat }); }
    
    foreach(QString pattern, functions) { highlightingRules.append({ QRegularExpression("\\b" + pattern + "\\b(?=\\()"), functionFormat }); }

    highlightingRules.append({ QRegularExpression("^\\s*#[^\n]*"), preprocessorFormat });

    highlightingRules.append({ QRegularExpression("//[^\n]*"), commentFormat });
}

void GLSLSyntaxHlighter::highlightBlock(const QString &text)
{
    foreach(HighlightingRule rule, highlightingRules)
    {
        QRegularExpression expression(rule.pattern);

        auto match = expression.globalMatch(text);
        while (match.hasNext())
        {
            auto currentMatch = match.next();
            setFormat(currentMatch.capturedStart(), currentMatch.capturedLength(), rule.format);
        }
    }

    // multiline comment management
    QRegularExpression commentStartExpression("/\\*");
    QRegularExpression commentEndExpression("\\*/");

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
            commentLength = endIndex - startIndex + 2;
        }

        setFormat(startIndex, commentLength, commentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }

    storeParenthesesData(text);
}

void GLSLSyntaxHlighter::storeParenthesesData(const QString& text)
{
    const QVector<char> parentheses({ '(', ')', '{', '}', '[', ']' });

    TextBlockData* data = new TextBlockData;

    foreach(auto parenthesis, parentheses)
    {
        int findPos = text.indexOf(parenthesis);
        while (findPos != -1)
        {
            data->parentheses.append({ parenthesis, findPos });
            findPos = text.indexOf(parenthesis, findPos + 1);
        }
    }

    setCurrentBlockUserData(data);   
}
