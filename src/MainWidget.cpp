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
#include "MainWidget.h"

MainWidget::MainWidget(QWidget* parent)
	: QMainWindow(parent)
    , m_textVert(nullptr)
    , m_textFrag(nullptr)
    , m_textVertHL(nullptr)
    , m_textFragHL(nullptr)
    , glOutput(nullptr)
{
	ui.setupUi(this);

	// QCodeEdit setup
	setupEditor();

	// Actions setup
	setupActions();

	// initial view: empty new shader
	ui.treeShaderStruct->setColumnCount(1);
	QList<QTreeWidgetItem*> items;
    items.append(new QTreeWidgetItem((QTreeWidget*)nullptr, QStringList(QString("Workspace"))));
	ui.treeShaderStruct->insertTopLevelItems(0, items);

	// example of child
	//ui.treeShaderStruct->insertTopLevelItem(1, new QTreeWidgetItem(items.at(0), QStringList(QString("Bla"))));

	// add OpenGL output
	setupGLPreview();
}

MainWidget::~MainWidget()
{
    delete glOutput;

    delete m_textVertHL;
    delete m_textFragHL;

    delete m_textVert;
    delete m_textFrag;
}

void MainWidget::logMessage(const QString& msg)
{
	ui.textCompileLog->appendPlainText(QDateTime::currentDateTime().toString("[hh:mm:ss] ") + msg);
}

void MainWidget::setupGLPreview()
{
	glOutput = new PreviewWidget(0);
	ui.frameGLPreview->setLayout(new QGridLayout());
	((QGridLayout*)ui.frameGLPreview->layout())->addWidget(glOutput);
}

void MainWidget::loadFile()
{
    fileProjectName = QFileDialog::getOpenFileName(this,
                                                   tr("Load Project"),
                                                   QDir::currentPath(),
                                                   tr("QRenderOrang Project (*.qrfx)"));

    // TODO: add support for loading project files
}

void MainWidget::loadDefaultShader()
{
	static QString phongVert =
	"// (C) Clockworkcoders Tutorials\n"
	"// http://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php\n\n"
	"varying vec3 N;\n"
	"varying vec3 v;\n\n"
	"void main(void)\n"
	"{\n"
	"\tv = vec3(gl_ModelViewMatrix * gl_Vertex);\n"
	"\tN = normalize(gl_NormalMatrix * gl_Normal);\n\n"
	"\tgl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
	"}";

	static QString phongFrag =
	"// (C) Clockworkcoders Tutorials\n"
	"// http://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php\n\n"
	"varying vec3 N;\n"
	"varying vec3 v;\n\n"
	"void main(void)\n"
	"{\n"
	"\tvec3 L = normalize(gl_LightSource[0].position.xyz - v);\n"
	"\tvec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)\n"
	"\tvec3 R = normalize(-reflect(L,N));\n\n"
	"\t//calculate Ambient Term:\n"
	"\tvec4 Iamb = gl_FrontLightProduct[0].ambient;\n\n"
	"\t//calculate Diffuse Term:\n"
	"\tvec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);\n"
	"\tIdiff = clamp(Idiff, 0.0, 1.0);\n\n"
	"\t// calculate Specular Term:\n"
	"\tvec4 Ispec = gl_FrontLightProduct[0].specular * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);\n"
	"\tIspec = clamp(Ispec, 0.0, 1.0);\n\n"
	"\t// write Total Color:\n"
	"\tgl_FragColor = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;\n"
	"}\n";

    if (m_textVert && m_textFrag)
    {
        m_textVert->setText(phongVert);
        m_textFrag->setText(phongFrag);
	}
}

void MainWidget::compileShader()
{
    if (glOutput && glOutput->isInitialized())
    {
        Shader* s = new Shader();
        s->setSource(m_textVert->toPlainText(), m_textFrag->toPlainText());
		glOutput->setShader(s);
		s->compileAndLink();
		glOutput->updateGL();

		logMessage("Compiling... " + s->compilationLog());
    }
    else
    {
		logMessage("No GL windows enabled, shader compilation aborted!");
	}
}

void MainWidget::setupActions()
{
	// Menu
    connect(ui.action_Open,       SIGNAL(triggered()), this, SLOT(loadFile()));
    connect(ui.action_Quit,       SIGNAL(triggered()), this, SLOT(close()));
    connect(ui.action_Compile,    SIGNAL(triggered()), this, SLOT(compileShader()));
    connect(ui.action_Qt,         SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui.action_QRO,        SIGNAL(triggered()), this, SLOT(about()));
	connect(ui.action_LoadSample, SIGNAL(triggered()), this, SLOT(loadDefaultShader()));
}

void MainWidget::setupEditor()
{
    m_textVert = new QTextEdit(ui.tabEditorVert);
    m_textFrag = new QTextEdit(ui.tabEditorFrag);

    m_textVert->setAcceptRichText(true);
    m_textFrag->setAcceptRichText(true);

    m_textVert->setWindowTitle("untitled.vert [*]");
    m_textFrag->setWindowTitle("untitled.frag [*]");

    /*
	m_textVert->addPanel("Line Number Panel", QCodeEdit::West, true)
			  ->setShortcut(QKeySequence("F11"));

	m_textFrag->addPanel("Line Number Panel", QCodeEdit::West, true)
              ->setShortcut(QKeySequence("F11"));
    */

    QFont fixedfont;
    fixedfont.setFamily("Consolas");
    fixedfont.setFixedPitch(true);
    fixedfont.setPointSize(10);
    m_textVert->setFont(fixedfont);
    m_textFrag->setFont(fixedfont);

    m_textVertHL = new GLSLSynHlighter(m_textVert->document());
    m_textFragHL = new GLSLSynHlighter(m_textFrag->document());

	ui.tabEditorVert->setLayout(new QGridLayout());
    ((QGridLayout*)ui.tabEditorVert->layout())->addWidget(m_textVert);

	ui.tabEditorFrag->setLayout(new QGridLayout());
    ((QGridLayout*)ui.tabEditorFrag->layout())->addWidget(m_textFrag);
}

void MainWidget::about()
{
	QMessageBox::about(this,
					   tr("About QRenderOrang..."),
					   tr("<h2>QRenderOrang SVN [dev only release]</h2>"
						  "<p>QRenderOrang is a multiplatform IDE for GLSL "
						  "shader authoring inspired by AMD RenderMonkey"
						  "&trade;</p>"
						  "<p>&copy; 2009 Carlo Casta "
						  "<a href=\"mailto:carlo.casta@gmail.com\">"
						  "carlo.casta@gmail.com</a></p>")
					   );
}


/*********************************
* GLSL Syntax Highlighter Class *
*********************************/

GLSLSynHlighter::GLSLSynHlighter(QTextDocument *parent)
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
    builtins << "gl_Position" << "gl_ClipSize" << "gl_ClipVertex"
        << "gl_Vertex" << "gl_Normal" << "gl_Color" << "gl_FragColor"
        << "gl_SecondaryColor" << "gl_FogCoord" << "gl_MultiTexCoord[0-7]"
        << "gl_FrontColor" << "gl_BackColor" << "gl_FrontSecondaryColor"
        << "gl_BackSecondaryColor" << "gl_TexCoord" << "gl_FogFragCoord"
        << "gl_FragData" << "gl_FrontFacing" << "gl_FragCoord"
        << "gl_FragDepth" << "gl_ModelViewMatrix" << "gl_ProjectionMatrix"
        << "gl_ModelViewProjectionMatrix" << "gl_ModelViewMatrixInverse"
        << "gl_ModelViewProjectionMatrixInverse" << "gl_NormalMatrix"
        << "gl_NormalScale" << "gl_DepthRange" << "gl_Point"
        << "gl_ModelViewMatrixInverseTranspose" << "gl_LightSource"
        << "gl_ModelViewMatrixTranspose" << "gl_ProjectionMatrixTranspose"
        << "gl_ModelViewProjectionMatrixInverseTranspose" << "gl_Fog"
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
}

void GLSLSynHlighter::highlightBlock(const QString &text)
{
    foreach(HighlightingRule rule, highlightingRules)
    {
        QRegExp expression(rule.pattern);
        int index = text.indexOf(expression);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = text.indexOf(expression, index + length);
        }
    }

    /* multiline comment management */
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        int endIndex = text.indexOf(commentEndExpression, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else {
            commentLength = endIndex - startIndex +
                commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression,
            startIndex + commentLength);
    }
}
