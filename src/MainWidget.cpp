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
#include "UniformWidget.h"
#include <QElapsedTimer>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

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
    LOG("MainWindow initialized");

    newProject();

    // ensure equal size btw editor/uniforms/GL
    ui.splitter->setSizes({ 1000, 1000, 1000 });
}

void MainWidget::setupGLPreview()
{
    QSurfaceFormat glFormat;
    glFormat.setVersion(3, 3);
    glFormat.setProfile(QSurfaceFormat::CompatibilityProfile);
    glFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    glFormat.setColorSpace(QSurfaceFormat::sRGBColorSpace);
    glFormat.setSamples(4);
    
	glWidget = new PreviewWidget(glFormat, this);
    glWidget->makeCurrent();

	ui.frameGLPreview->setLayout(new QGridLayout());
	((QGridLayout*)ui.frameGLPreview->layout())->addWidget(glWidget);
}

void MainWidget::updateWindowTitle()
{
    auto title = QString("QRenderOrang");
    if (!fileProjectName.isEmpty())
    {
        title += " - " + fileProjectName;
    }

    setWindowTitle(title);
}

void MainWidget::newProject()
{
    fileProjectName.clear();
    updateWindowTitle();

    // load default shader in text editor

    m_textVert->setText(R"vs(#version 330

// available builtin vertex data and locations
layout (location = 0) in vec3 position;
//layout (location = 1) in vec3 normal;
//layout (location = 2) in vec2 uv;
//layout (location = 3) in vec3 tangent;
//layout (location = 4) in vec3 bitangent;

// available builtin matrices
uniform struct
{
	//mat4 projection;
	//mat4 view;
	//mat4 model;
    mat4 MVP;
} matrix;

void main()
{
	gl_Position = matrix.MVP * vec4(position, 1);
})vs");

    m_textFrag->setText(R"fs(#version 330

out vec4 outColor;

void main()
{
	outColor = vec4(1,1,1,1);
})fs");

    // compile default shader with default mesh
    loadBuiltinMesh(Mesh::SPHERE);
    compileShader();
}

void MainWidget::loadProject()
{
    fileProjectName = QFileDialog::getOpenFileName(this,
                                                   tr("Load Project"),
                                                   "",
                                                   tr("QRenderOrang Project (*.qrfx)"));

    if (!fileProjectName.isEmpty())
    {
        QFile file(fileProjectName);
        if (file.open(QIODevice::ReadOnly))
        {
            QByteArray projectRawData = file.readAll();
            QJsonDocument projectDocument(QJsonDocument::fromJson(projectRawData));
            QJsonObject projectObject = projectDocument.object();

            // #TODO check if save version number match - now there is no need to

            m_textVert->clear();
            if (projectObject.contains("vs"))
            {
                m_textVert->setPlainText(projectObject["vs"].toString());
            }
            
            m_textFrag->clear();
            if (projectObject.contains("fs"))
            {
                m_textFrag->setPlainText(projectObject["fs"].toString());
            }

            if (projectObject.contains("mesh"))
            {
                Mesh* newMesh = new Mesh();
                newMesh->load(projectObject["mesh"].toObject());
                glWidget->setMesh(newMesh);
            }

            removeAllUniforms();
            if (projectObject.contains("uniforms") && projectObject["uniforms"].isArray())
            {
                for (auto uniform : projectObject["uniforms"].toArray())
                {
                    auto newUniform = addUniform();
                    newUniform->load(uniform.toObject());
                }
            }

            compileShader();

            auto projectName = QFileInfo(fileProjectName).fileName();
            LOG(QString("Loaded project %1").arg(projectName));
            updateWindowTitle();
        }
    }
}

void MainWidget::saveProjectAs()
{
    if (!fileProjectName.isEmpty())
    {
        auto projectFileName = QFileDialog::getSaveFileName(this, tr("Save Project"), "", tr("QRenderOrang Project (*.qrfx)"));
        if (!projectFileName.isEmpty())
        {
            fileProjectName = projectFileName;
        }
    }
    
    saveProject();
}

void MainWidget::saveProject()
{
    auto projectFileName = fileProjectName.isEmpty()? QFileDialog::getSaveFileName(this,
                                                                                   tr("Save Project"),
                                                                                   "",
                                                                                   tr("QRenderOrang Project (*.qrfx)"))
                                                    : fileProjectName;

    QFile file(projectFileName);
    if (!projectFileName.isEmpty() && file.open(QIODevice::WriteOnly))
    {
        QJsonObject projectData;

        projectData["version"] = "1";
        projectData["mesh"] = glWidget->mesh()->save();
        projectData["vs"] = m_textVert->toPlainText();
        projectData["fs"] = m_textFrag->toPlainText();

        QJsonArray activeUniforms;
        for (auto child : ui.uniformList->widget()->children())
        {
            if (auto uniform = qobject_cast<UniformWidget*>(child))
            {
                activeUniforms.append(uniform->save());
            }
        }
        projectData["uniforms"] = activeUniforms;

        file.write(QJsonDocument(projectData).toJson());
        fileProjectName = projectFileName;

        updateWindowTitle();
    }
}

void MainWidget::compileShader()
{
    glWidget->setShader(m_textVert->toPlainText(), m_textFrag->toPlainText());
}

void MainWidget::setupActions()
{
    // Menu
    connect(ui.action_New,        SIGNAL(triggered()), this, SLOT(newProject()));
    connect(ui.action_Open,       SIGNAL(triggered()), this, SLOT(loadProject()));
    connect(ui.action_Save,       SIGNAL(triggered()), this, SLOT(saveProject()));
    connect(ui.actionSave_As,     SIGNAL(triggered()), this, SLOT(saveProjectAs()));
    connect(ui.action_Quit,       SIGNAL(triggered()), this, SLOT(close()));
    
    connect(ui.action_Compile,    SIGNAL(triggered()), this, SLOT(compileShader()));
    connect(ui.actionAdd_Uniform, SIGNAL(triggered()), this, SLOT(addUniform()));
    
    connect(ui.action_Qt,         SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui.action_QRO,        SIGNAL(triggered()), this, SLOT(about()));

    connect(ui.actionSphere,      &QAction::triggered, this, [this]{ loadBuiltinMesh(Mesh::SPHERE); });
    connect(ui.actionCube,        &QAction::triggered, this, [this]{ loadBuiltinMesh(Mesh::CUBE); });
    connect(ui.actionImport,      SIGNAL(triggered()), this, SLOT(loadMesh()));

    connect(ui.actionWireframe,   &QAction::triggered, this, [this]{ ui.actionWireframe->setChecked(glWidget->toggleWireframe()); });
    connect(ui.actionUnlit,       &QAction::triggered, this, [this]{ ui.actionUnlit->setChecked(glWidget->toggleUnlit()); });
}

UniformWidget* MainWidget::addUniform()
{
    UniformWidget* newUniform = new UniformWidget(glWidget);
    ui.uniformList->widget()->layout()->addWidget(newUniform);
    connect(newUniform, &UniformWidget::deleted, this, &MainWidget::removeUniform);

    return newUniform;
}

void MainWidget::removeUniform(UniformWidget* target)
{
    ui.uniformList->widget()->layout()->removeWidget(target);
    delete target;
}

void MainWidget::removeAllUniforms()
{
    QVector<QWidget*> widgets;
    for (auto child : ui.uniformList->widget()->children())
    {
        if (auto uniform = qobject_cast<UniformWidget*>(child))
        {
            widgets.push_back(uniform);
        }
    }

    for (auto child : widgets)
    {
        ui.uniformList->widget()->layout()->removeWidget(child);
        delete child;
    }
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
