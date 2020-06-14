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
#include <QOpenGLFunctions_3_3_Compatibility>
#include "Common.h"

template<class T>
using OnTextureLoaded = std::function<void(const T&, const QImage&)>;

class Texture
{
public:
	Texture();
	~Texture();

	Texture(Texture&) = delete;
	Texture(Texture&&) = delete;

	Texture& operator=(Texture&) = delete;
	Texture& operator=(Texture&&) = delete;

	bool load(const QString& filename, const OnTextureLoaded<Texture>& callback = [](const Texture&, const QImage&){});
	bool load(uint8_t* buffer, int len, const vec3& dim, const OnTextureLoaded<Texture>& callback = [](const Texture&, const QImage&) {});
	void unload();
	
	void onLoad(const uint8_t* pixelsRGBA) const;

	bool valid() const          { return isValid; }
	int width() const           { return size.x; }
	int height() const          { return size.y; }
	int depth() const           { return size.z; }
	uint8_t* pixels() const     { return data; }
	int pixelsSize() const      { return dataSize; }

	bool compressed() const     { return isCompressed; }
	GLenum glDataType() const   { return type; }
	GLint glFormat() const      { return format; }
	GLint glsRGBFormat() const  { return sRGBFormat; }

private:
	bool isValid;
	vec3 size;
	uint8_t* data;
	int dataSize;
	GLenum type;
	GLint format;
	GLint sRGBFormat;
	bool isCompressed;

	OnTextureLoaded<Texture> onLoadedCallback;
};
