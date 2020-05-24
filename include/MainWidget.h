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
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QDate>

#include "PreviewWidget.h"
#include "Mesh.h"
#include "ui_qrenderorang.h"

class ShaderEditor;
class GLSLSyntaxHlighter;
class UniformWidget;

class MainWidget : public QMainWindow
{
	Q_OBJECT

public:
	MainWidget(QWidget *parent = 0);

public slots:
	void newProject();
	void loadProject();
	void saveProjectAs();
	void saveProject();
	
	void compileShader();
    
	void loadBuiltinMesh(Mesh::MeshType type);
    void importMesh();
    
	UniformWidget* addUniform();
    void removeUniform(UniformWidget* target);
	void removeAllUniforms();

	void about();

private:
	void setupActions();
	void setupGLPreview();
	void setupEditor();
	void updateWindowTitle();

	QString fileProjectName;
	Ui::QRenderOrangGUI ui;
    ShaderEditor* m_textVert;
    ShaderEditor* m_textFrag;
	PreviewWidget* glWidget;
    GLSLSyntaxHlighter* m_textVertHL;
    GLSLSyntaxHlighter* m_textFragHL;
};
