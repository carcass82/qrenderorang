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

	bool valid() const                 { return isValid; }
	int width(int mip = 0) const       { return getMipDimension(mip).x; }
	int height(int mip = 0) const      { return getMipDimension(mip).y; }
	int depth(int mip = 0) const       { return getMipDimension(mip).z; }
	uint8_t* pixels(int mip = 0) const { return &data[getMipOffset(mip)]; }
	int pixelsSize(int mip = 0) const  { return getMipSize(mip); }
	int getMipmapCount() const         { return mipmaps; }

	bool compressed() const            { return isCompressed; }
	GLenum glDataType() const          { return type; }
	GLint glFormat() const             { return format; }
	GLint glsRGBFormat() const         { return sRGBFormat; }

	GLint getMinFilter() const         { return minFilter; }
	GLint getMagFilter() const         { return magFilter; }
	GLint getWrapMode() const          { return wrapMode; }

	enum Format { UNSUPPORTED = -1, RGBA, RGBA16, DXT1, DXT5 };

private:
	int getMipSize(int mipLevel) const;
	int getMipOffset(int mipLevel) const;
	vec3 getMipDimension(int mipLevel) const;

	Format texFormat;
	bool isValid;
	vec3 size;
	uint8_t* data;
	int dataSize;
	GLenum type;
	GLint format;
	GLint sRGBFormat;
	bool isCompressed;
	int mipmaps;
	GLint minFilter;
	GLint magFilter;
	GLint wrapMode;

	OnTextureLoaded<Texture> onLoadedCallback;
};
