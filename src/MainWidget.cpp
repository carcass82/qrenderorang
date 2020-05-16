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
#include "SyntaxHighlighter.h"
#include <QElapsedTimer>

MainWidget::MainWidget(QWidget* parent)
	: QMainWindow(parent)
    , m_textVert(nullptr)
    , m_textFrag(nullptr)
    , glWidget(nullptr)
    , m_textVertHL(nullptr)
    , m_textFragHL(nullptr)
{
	ui.setupUi(this);
    ui.textCompileLog->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

	// QCodeEdit setup
	setupEditor();

	// Actions setup
	setupActions();
	
	// add OpenGL output
	setupGLPreview();

    // register LOG service
    Logger::get().setOutputWidget(ui.textCompileLog);

    // and test it!
    LOG("MainWindow initialized");
}

void MainWidget::setupGLPreview()
{
    QSurfaceFormat glFormat;
    glFormat.setVersion(3, 3);
    glFormat.setProfile(QSurfaceFormat::CompatibilityProfile);
    glFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    glFormat.setColorSpace(QSurfaceFormat::sRGBColorSpace);
    
	glWidget = new PreviewWidget(glFormat, this);
    glWidget->makeCurrent();

	ui.frameGLPreview->setLayout(new QGridLayout());
	((QGridLayout*)ui.frameGLPreview->layout())->addWidget(glWidget);
}

void MainWidget::loadFile()
{
    fileProjectName = QFileDialog::getOpenFileName(this,
                                                   tr("Load Project"),
                                                   QDir::currentPath(),
                                                   tr("QRenderOrang Project (*.qrfx)"));

    // #TODO: add support for project files
}

void MainWidget::loadDefaultShader()
{
    static const QString phongVert =
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

    static const QString phongFrag =
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
    glWidget->setShader(m_textVert->toPlainText(), m_textFrag->toPlainText());
}

void MainWidget::setupActions()
{
    // Menu
    connect(ui.action_Open,       SIGNAL(triggered()), this, SLOT(loadFile()));
    connect(ui.action_Quit,       SIGNAL(triggered()), this, SLOT(close()));
    connect(ui.action_LoadSample, SIGNAL(triggered()), this, SLOT(loadDefaultShader()));
    connect(ui.action_Compile,    SIGNAL(triggered()), this, SLOT(compileShader()));
    connect(ui.action_Qt,         SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui.action_QRO,        SIGNAL(triggered()), this, SLOT(about()));

    connect(ui.actionSphere,      &QAction::triggered, this, [this]{ loadBuiltinMesh(Mesh::SPHERE); });
    connect(ui.actionCube,        &QAction::triggered, this, [this]{ loadBuiltinMesh(Mesh::CUBE); });
    connect(ui.actionImport,      SIGNAL(triggered()), this, SLOT(loadMesh()));

    connect(ui.actionWireframe,   &QAction::triggered, this, [this]{ ui.actionWireframe->setChecked(glWidget->toggleWireframe()); });
    connect(ui.actionUnlit,       &QAction::triggered, this, [this]{ ui.actionUnlit->setChecked(glWidget->toggleUnlit()); });
}

void MainWidget::loadBuiltinMesh(Mesh::MeshType type)
{
    Mesh* newMesh = (new Mesh())->Load(type);
    LOG(QString("Loaded Mesh %1 (%2 Vertices / %3 Triangles)").arg(
                Mesh::typeToString(type),
                QString::number(newMesh->numVertices()),
                QString::number(newMesh->numTriangles())));

    glWidget->setMesh(newMesh);
}

void MainWidget::loadMesh()
{
    QString meshFile = QFileDialog::getOpenFileName(this,
                                                    tr("Load Mesh"),
                                                    QDir::currentPath(),
                                                    tr("Wavefront OBJ (*.obj)"));

    if (!meshFile.isEmpty() && glWidget)
    {
        QElapsedTimer timer;
        timer.start();

        Mesh* newMesh = (new Mesh())->Load(meshFile);

        LOG(QString("Loaded Mesh \"%1\" (%2 Vertices / %3 Triangles) in %4 seconds").arg(
                    QFileInfo(meshFile).fileName(),
                    QString::number(newMesh->numVertices()),
                    QString::number(newMesh->numTriangles()),
                    QString::number(timer.elapsed() / 1000.f, 'g', 2)));

        glWidget->setMesh(newMesh);
    }
}

void MainWidget::setupEditor()
{
    m_textVert = new QTextEdit(ui.tabEditorVert);
    m_textFrag = new QTextEdit(ui.tabEditorFrag);

    m_textVert->setAcceptRichText(true);
    m_textFrag->setAcceptRichText(true);

    m_textVert->setWordWrapMode(QTextOption::WrapMode::NoWrap);
    m_textFrag->setWordWrapMode(QTextOption::WrapMode::NoWrap);

    m_textVert->setWindowTitle("untitled.vert");
    m_textFrag->setWindowTitle("untitled.frag");

    const QFont fixedfont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    m_textVert->setFont(fixedfont);
    m_textFrag->setFont(fixedfont);

    QFontMetrics metrics(fixedfont);
    m_textVert->setTabStopWidth(4 * metrics.width(' '));
    m_textFrag->setTabStopWidth(4 * metrics.width(' '));

    m_textVertHL = new GLSLSyntaxHlighter(m_textVert->document());
    m_textFragHL = new GLSLSyntaxHlighter(m_textFrag->document());

	ui.tabEditorVert->setLayout(new QGridLayout());
    ((QGridLayout*)ui.tabEditorVert->layout())->addWidget(m_textVert);

	ui.tabEditorFrag->setLayout(new QGridLayout());
    ((QGridLayout*)ui.tabEditorFrag->layout())->addWidget(m_textFrag);
}

void MainWidget::about()
{
	QMessageBox::about(this,
					   tr("About QRenderOrang..."),
                       tr("<h2>QRenderOrang [dev only release]</h2>"
						  "<p>QRenderOrang is a multiplatform IDE for GLSL "
						  "shader authoring inspired by AMD RenderMonkey"
						  "&trade;</p>"
						  "<p>&copy; 2009 Carlo Casta "
						  "<a href=\"mailto:carlo.casta@gmail.com\">"
						  "carlo.casta@gmail.com</a></p>")
					   );
}
