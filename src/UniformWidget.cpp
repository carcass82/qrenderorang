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
#include "UniformWidget.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

UniformWidget::UniformWidget(const QString& name, UniformType type, PreviewWidget* glWidget, QWidget* parent)
	: QWidget(parent)
	, GLWidget(glWidget)
{
	ui.setupUi(this);

	uniformName = name;
	uniformType = type;

	Row0.push_back(ui.uniformX0); Row0.push_back(ui.uniformY0); Row0.push_back(ui.uniformZ0); Row0.push_back(ui.uniformW0);
	Row1.push_back(ui.uniformX1); Row1.push_back(ui.uniformY1); Row1.push_back(ui.uniformZ1); Row1.push_back(ui.uniformW1);
	Row2.push_back(ui.uniformX2); Row2.push_back(ui.uniformY2); Row2.push_back(ui.uniformZ2); Row2.push_back(ui.uniformW2);
	Row3.push_back(ui.uniformX3); Row3.push_back(ui.uniformY3); Row3.push_back(ui.uniformZ3); Row3.push_back(ui.uniformW3);

	ui.uniformName->setText(name);
	ui.uniformType->setCurrentIndex(type);

	connect(ui.uniformName,                           &QLineEdit::editingFinished,       this, [&]()          { updateNameAndType(); });
	connect(ui.uniformType,            qOverload<int>(&QComboBox::currentIndexChanged),  this, [&](int)       { updateNameAndType(); });
	connect(ui.uniformX0,           qOverload<double>(&QDoubleSpinBox::valueChanged),    this, [&](double)    { updateShaderValue(); });
	connect(ui.uniformY0,           qOverload<double>(&QDoubleSpinBox::valueChanged),    this, [&](double)    { updateShaderValue(); });
	connect(ui.uniformZ0,           qOverload<double>(&QDoubleSpinBox::valueChanged),    this, [&](double)    { updateShaderValue(); });
	connect(ui.uniformW0,           qOverload<double>(&QDoubleSpinBox::valueChanged),    this, [&](double)    { updateShaderValue(); });
	connect(ui.uniformColorPreview,                   &QLabel::linkActivated,            this, [&]()          { chooseColor(); });
	connect(ui.uniformColorSRGB,                      &QCheckBox::clicked,               this, [&]()          { updateShaderValue(); });
	connect(ui.uniformTextureOpen,                    &QPushButton::clicked,             this, [&]()          { chooseTexture(); });
	connect(ui.uniformTextureSRGB,                    &QCheckBox::clicked,               this, [&]()          { updateShaderValue(); });
	connect(ui.deleteButton,                          &QPushButton::clicked,             this, [&]()          { emit deleted(this); });
	connect(GLWidget,                                 &QWidget::destroyed,               this, [&]()          { GLWidget = nullptr; });

	updateUI();
}

UniformWidget::~UniformWidget()
{
	if (GLWidget)
	{
		resetShaderValue();
	}
}

void UniformWidget::load(const QJsonObject& data)
{
	if (data.contains("name") && data.contains("type") && data.contains("value"))
	{
		UniformType type = (UniformType)(data["type"].toInt() < UniformType::MAX? data["type"].toInt() : 0); data["type"].toInt();
		QString value = data["value"].toString();
		QString flags = data.contains("flags")? data["flags"].toString() : "";

		ui.uniformName->setText(data["name"].toString());
		ui.uniformType->setCurrentIndex(data["type"].toInt());
		updateNameAndType();

		switch (type)
		{
		case Float:
			ui.uniformX0->setValue(value.toDouble());
			break;

		case Vec2:
		{
			auto vecData = value.split(" ");
			if (vecData.size() == 2)
			{
				ui.uniformX0->setValue(vecData[0].toDouble());
				ui.uniformY0->setValue(vecData[1].toDouble());
			}
			break;
		}

		case Vec3:
		{
			auto vecData = value.split(" ");
			if (vecData.size() == 3)
			{
				ui.uniformX0->setValue(vecData[0].toDouble());
				ui.uniformY0->setValue(vecData[1].toDouble());
				ui.uniformZ0->setValue(vecData[2].toDouble());
			}
			break;
		}

		case Vec4:
		{
			auto vecData = value.split(" ");
			if (vecData.size() == 4)
			{
				ui.uniformX0->setValue(vecData[0].toDouble());
				ui.uniformY0->setValue(vecData[1].toDouble());
				ui.uniformZ0->setValue(vecData[2].toDouble());
				ui.uniformW0->setValue(vecData[3].toDouble());
			}
			break;
		}

		case Mat3:
		{
			auto vecData = value.split(" ");
			if (vecData.size() == 9)
			{
				ui.uniformX0->setValue(vecData[0].toDouble());
				ui.uniformY0->setValue(vecData[1].toDouble());
				ui.uniformZ0->setValue(vecData[2].toDouble());

				ui.uniformX1->setValue(vecData[3].toDouble());
				ui.uniformY1->setValue(vecData[4].toDouble());
				ui.uniformZ1->setValue(vecData[5].toDouble());

				ui.uniformX2->setValue(vecData[6].toDouble());
				ui.uniformY2->setValue(vecData[7].toDouble());
				ui.uniformZ2->setValue(vecData[8].toDouble());
			}
			break;
		}

		case Mat4:
		{
			auto vecData = value.split(" ");
			if (vecData.size() == 16)
			{
				ui.uniformX0->setValue(vecData[0].toDouble());
				ui.uniformY0->setValue(vecData[1].toDouble());
				ui.uniformZ0->setValue(vecData[2].toDouble());
				ui.uniformW0->setValue(vecData[3].toDouble());

				ui.uniformX1->setValue(vecData[4].toDouble());
				ui.uniformY1->setValue(vecData[5].toDouble());
				ui.uniformZ1->setValue(vecData[6].toDouble());
				ui.uniformW1->setValue(vecData[7].toDouble());

				ui.uniformX2->setValue(vecData[8].toDouble());
				ui.uniformY2->setValue(vecData[9].toDouble());
				ui.uniformZ2->setValue(vecData[10].toDouble());
				ui.uniformW2->setValue(vecData[11].toDouble());

				ui.uniformX3->setValue(vecData[12].toDouble());
				ui.uniformY3->setValue(vecData[13].toDouble());
				ui.uniformZ3->setValue(vecData[14].toDouble());
				ui.uniformW3->setValue(vecData[15].toDouble());
			}
			break;
		}

		case Color:
			ui.uniformColorSRGB->setChecked(flags == "sRGB");
			chooseColor(QColor(value));
			break;

		case Texture:
		{
			if (!value.isEmpty())
			{
				ui.uniformTextureSRGB->setChecked(flags == "sRGB");
				chooseTexture(value);
			}
			break;
		}

		}
	}
}

QJsonObject UniformWidget::save() const
{
	QJsonObject uniform;
	uniform["name"] = uniformName;
	uniform["type"] = uniformType;

	switch (uniformType)
	{
	case Float:
		uniform["value"] = QString::number(ui.uniformX0->value());
		break;
	case Vec2:
		uniform["value"] = QString("%1 %2").arg(QString::number(ui.uniformX0->value()),
												QString::number(ui.uniformY0->value()));
		break;
	case Vec3:
		uniform["value"] = QString("%1 %2 %3").arg(QString::number(ui.uniformX0->value()),
												   QString::number(ui.uniformY0->value()),
												   QString::number(ui.uniformZ0->value()));
		break;
	case Vec4:
		uniform["value"] = QString("%1 %2 %3 %4").arg(QString::number(ui.uniformX0->value()),
													  QString::number(ui.uniformY0->value()),
													  QString::number(ui.uniformZ0->value()),
													  QString::number(ui.uniformW0->value()));
		break;
	case Mat3:
		uniform["value"] = QString("%1 %2 %3 %4 %5 %6 %7 %8 %9").arg(QString::number(ui.uniformX0->value()),
																	 QString::number(ui.uniformY0->value()),
																	 QString::number(ui.uniformZ0->value()),

																	 QString::number(ui.uniformX1->value()),
																	 QString::number(ui.uniformY1->value()),
																	 QString::number(ui.uniformZ1->value()),

																	 QString::number(ui.uniformX2->value()),
																	 QString::number(ui.uniformY2->value()),
																	 QString::number(ui.uniformZ2->value()));

		break;
	case Mat4:
		uniform["value"] = QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14 %15 %16").arg(QString::number(ui.uniformX0->value()),
																								 QString::number(ui.uniformY0->value()),
																								 QString::number(ui.uniformZ0->value()),
																								 QString::number(ui.uniformW0->value()),

																								 QString::number(ui.uniformX1->value()),
																								 QString::number(ui.uniformY1->value()),
																								 QString::number(ui.uniformZ1->value()),
																								 QString::number(ui.uniformW1->value()),

																								 QString::number(ui.uniformX2->value()),
																								 QString::number(ui.uniformY2->value()),
																								 QString::number(ui.uniformZ2->value()),
																								 QString::number(ui.uniformW2->value()),

																								 QString::number(ui.uniformX3->value()),
																								 QString::number(ui.uniformY3->value()),
																								 QString::number(ui.uniformZ3->value()),
																								 QString::number(ui.uniformW3->value()));
		break;

	case Color:
	{
		uniform["value"] = ui.uniformColorPreview->palette().background().color().name(QColor::HexArgb);
		uniform["flags"] = ui.uniformColorSRGB->isChecked()? "sRGB" : "RGB";
		break;
	}

	case Texture:
	{
		uniform["value"] = ui.uniformTexturePath->text();
		uniform["flags"] = ui.uniformTextureSRGB->isChecked()? "sRGB" : "RGB";
		break;
	}
	}

	return uniform;
}

void UniformWidget::updateShaderValue()
{
	switch (uniformType)
	{
	case Float:
		GLWidget->setShaderParameter(uniformName, (float)ui.uniformX0->value());
		break;
	case Vec2:
		GLWidget->setShaderParameter(uniformName, vec2(ui.uniformX0->value(), ui.uniformY0->value()));
		break;
	case Vec3:
		GLWidget->setShaderParameter(uniformName, vec3(ui.uniformX0->value(), ui.uniformY0->value(), ui.uniformZ0->value()));
		break;
	case Vec4:
		GLWidget->setShaderParameter(uniformName, vec4(ui.uniformX0->value(), ui.uniformY0->value(), ui.uniformZ0->value(), ui.uniformW0->value()));
		break;
	case Mat3:
		GLWidget->setShaderParameter(uniformName, mat3(vec3(ui.uniformX0->value(), ui.uniformY0->value(), ui.uniformZ0->value()),
										               vec3(ui.uniformX1->value(), ui.uniformY1->value(), ui.uniformZ1->value()),
										               vec3(ui.uniformX2->value(), ui.uniformY2->value(), ui.uniformZ2->value())));
		break;
	case Mat4:
		GLWidget->setShaderParameter(uniformName, mat4(vec4(ui.uniformX0->value(), ui.uniformY0->value(), ui.uniformZ0->value(), ui.uniformW0->value()),
										               vec4(ui.uniformX1->value(), ui.uniformY1->value(), ui.uniformZ1->value(), ui.uniformW1->value()),
										               vec4(ui.uniformX2->value(), ui.uniformY2->value(), ui.uniformZ2->value(), ui.uniformW2->value()),
										               vec4(ui.uniformX3->value(), ui.uniformY3->value(), ui.uniformZ3->value(), ui.uniformW3->value())));
		break;

	case Color:
	{
		QColor uiColor = ui.uniformColorPreview->palette().background().color();
		bool isSRGB = ui.uniformColorSRGB->isChecked();
		vec4 color(uiColor.redF(), uiColor.greenF(), uiColor.blueF(), uiColor.alphaF());
		GLWidget->setShaderParameter(uniformName, (isSRGB? linear(color) : color));
		break;
	}

	case Texture:
	{
		bool isSRGB = ui.uniformTextureSRGB->isChecked();
		auto texSize = ui.uniformTextureSize->text().split('x');
		if (texSize.size() == 2 && !uniformTextureData.isEmpty())
		{
			int width = texSize[0].toInt();
			int height = texSize[1].toInt();
			GLWidget->setShaderResource(uniformName, uniformTextureData.constData(), width, height, 4, isSRGB);
		}
		break;
	}

	default:
		break;
	}
}

void UniformWidget::resetUI()
{
	for (auto widget : Row0) widget->setVisible(false);
	for (auto widget : Row1) widget->setVisible(false);
	for (auto widget : Row2) widget->setVisible(false);
	for (auto widget : Row3) widget->setVisible(false);

	ui.uniformColorPreview->setVisible(false);
	ui.uniformColorSRGB->setVisible(false);

	ui.uniformTextureOpen->setVisible(false);
	ui.uniformTexturePreview->setVisible(false);
	ui.uniformTextureSize->setVisible(false);
	ui.uniformTextureSRGB->setVisible(false);
	ui.uniformTexturePath->setVisible(false);
}

void UniformWidget::updateUI()
{
	resetUI();

	switch (uniformType)
	{
	case Float: for (int i = 0; i < 1; ++i) Row0[i]->setVisible(true); break;

	case Vec2:  for (int i = 0; i < 2; ++i) Row0[i]->setVisible(true); break;

	case Vec3:  for (int i = 0; i < 3; ++i) Row0[i]->setVisible(true); break;

	case Vec4:  for (int i = 0; i < 4; ++i) Row0[i]->setVisible(true); break;

	case Mat3:  for (int i = 0; i < 3; ++i) Row0[i]->setVisible(true);
				for (int i = 0; i < 3; ++i) Row1[i]->setVisible(true);
				for (int i = 0; i < 3; ++i) Row2[i]->setVisible(true); break;

	case Mat4:  for (int i = 0; i < 4; ++i) Row0[i]->setVisible(true);
				for (int i = 0; i < 4; ++i) Row1[i]->setVisible(true);
				for (int i = 0; i < 4; ++i) Row2[i]->setVisible(true);
				for (int i = 0; i < 4; ++i) Row3[i]->setVisible(true); break;

	case Color:
		ui.uniformColorPreview->setVisible(true);
		ui.uniformColorSRGB->setVisible(true);
		break;

	case Texture:
		ui.uniformTextureOpen->setVisible(true);
		ui.uniformTexturePreview->setVisible(true);
		ui.uniformTextureSize->setVisible(true);
		ui.uniformTextureSRGB->setVisible(true);
		ui.uniformTexturePath->setVisible(true);
		break;

	default:
		break;
	}
}

void UniformWidget::resetShaderValue()
{
	switch (uniformType)
	{
	case Float:   GLWidget->setShaderParameter(uniformName, .0f, true);    break;
	case Vec2:    GLWidget->setShaderParameter(uniformName, vec2(), true); break;
	case Vec3:    GLWidget->setShaderParameter(uniformName, vec3(), true); break;
	case Vec4:    GLWidget->setShaderParameter(uniformName, vec4(), true); break;
	case Mat3:    GLWidget->setShaderParameter(uniformName, mat3(), true); break;
	case Mat4:    GLWidget->setShaderParameter(uniformName, mat4(), true); break;
	case Color:   GLWidget->setShaderParameter(uniformName, vec4(), true); break;
	case Texture: GLWidget->deleteShaderResource(uniformName);             break;
	}
}

void UniformWidget::updateNameAndType()
{
	resetShaderValue();

	uniformName = ui.uniformName->text();
	uniformType = (UniformType)ui.uniformType->currentIndex();

	updateUI();
	updateShaderValue();
}

void UniformWidget::chooseColor(const QColor& newColor)
{
	const QColor color = newColor.isValid()? newColor : QColorDialog::getColor(ui.uniformColorPreview->palette().background().color(), this, "Select Color", QColorDialog::ShowAlphaChannel);
	if (color.isValid())
	{
		ui.uniformColorPreview->setPalette(QPalette(color));
		ui.uniformColorPreview->setAutoFillBackground(true);

		updateShaderValue();
	}
}

void UniformWidget::chooseTexture(const QString& path)
{
	QString imagePath;
	if (path.isEmpty())
	{
		imagePath = QFileDialog::getOpenFileName(this, tr("Open Image"), QString(), tr("Images (*.jpg *.png *.tga *.bmp *.psd *.gif *.hdr *.pic *.png);;All files (*.*)"));
	}
	else
	{
		imagePath = path;
	}
	
    if (!imagePath.isEmpty())
    {
		ui.uniformTexturePath->setPalette(QApplication::palette(ui.uniformTexturePath));
		ui.uniformTexturePath->setText(imagePath);

        int width, height, n;
        unsigned char* pixels = stbi_load(imagePath.toStdString().c_str(), &width, &height, &n, 4);
        if (pixels)
        {
			uniformTextureData = QByteArray(reinterpret_cast<const char*>(pixels), width * height * 4);

            ui.uniformTexturePreview->setPixmap(QPixmap::fromImage(QImage(pixels, width, height, QImage::Format_RGBA8888).scaled(ui.uniformTexturePreview->size(), Qt::KeepAspectRatio)));
			ui.uniformTextureSize->setText(QString("%1x%2").arg(QString::number(width), QString::number(height)));

            stbi_image_free(pixels);

			updateShaderValue();
        }
		else
		{
			QPalette notFound;
			notFound.setColor(QPalette::Text, Qt::red);

			ui.uniformTexturePath->setPalette(notFound);
		}
    }
}
