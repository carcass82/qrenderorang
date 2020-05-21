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
#include <QColorDialog>
#include <QFileDialog>
#include <QJsonObject>

#include "Common.h"
#include "PreviewWidget.h"

#include "ui_uniform.h"

class UniformWidget : public QWidget
{
    Q_OBJECT

public:
    enum UniformType { Float, Vec2, Vec3, Vec4, Mat3, Mat4, Color, Texture, MAX };

    UniformWidget(const QString& name, UniformType type, PreviewWidget* glWidget, QWidget* parent = nullptr);
    
	UniformWidget(PreviewWidget* glWidget)
        : UniformWidget("", Float, glWidget)
    {}

    ~UniformWidget();

    QJsonObject save() const;
    void load(const QJsonObject& data);

signals:
    void deleted(UniformWidget* me);

private:
    void resetUI();
    void updateUI();

    void resetShaderValue();
    void updateShaderValue();

    void updateNameAndType();

    void chooseColor();
    void chooseTexture(const QString& path = "");


    Ui::UniformWidget ui;
    QString uniformName;
    UniformType uniformType = MAX;
    QByteArray uniformTextureData;
    PreviewWidget* GLWidget;
    
    QVector<QWidget*> Row0;
    QVector<QWidget*> Row1;
    QVector<QWidget*> Row2;
    QVector<QWidget*> Row3;
};
