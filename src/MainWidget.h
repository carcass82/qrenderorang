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

#include <GL/glew.h>

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QTreeWidgetItem>
#include <QDate>
#include "PreviewWidget.h"
#include "ui_qrenderorang.h"

class MainWidget : public QMainWindow
{
	Q_OBJECT

public:
	MainWidget(QWidget *parent = 0);
	~MainWidget() {}
	void logMessage(const QString&);

public slots:
	void loadDefaultShader();
	void loadFile();
	void about();
	void compileShader();

private:
	void setupActions();
	void setupGLPreview();
	void setupEditor();

	QString fileProjectName;
	Ui::QRenderOrangGUI ui;
    QTextEdit *m_textVert;
    QTextEdit *m_textFrag;
	PreviewWidget *glOutput;
};
