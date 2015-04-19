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
#include <GL/glew.h>
#include <QtGui>
#include <QApplication>
#include <QtWidgets>
#include "MainWidget.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

    QSplashScreen splash(QPixmap(":/images/splash.png"), Qt::WindowStaysOnTopHint);
    splash.showMessage("Loading QRenderOrang...", Qt::AlignBottom | Qt::AlignLeft, Qt::white);
	splash.show();

	MainWidget w;
	w.show();

	splash.finish(&w);

	return app.exec();
}
