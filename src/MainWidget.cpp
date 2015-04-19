/****************************************************************************
 *                                                                          *
 * QRenderOrang - an IDE for GLSL shader authoring                          *
 *                                                                          *
 * copyright (c) 2008 Carlo Casta <ccasta@users.sourceforge.net>            *
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
