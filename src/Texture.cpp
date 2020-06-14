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
#include "Texture.h"

namespace DDS
{

enum DDPFFlags
{
    DDPF_NONE        = 0,
    DDPF_ALPHAPIXELS = 0x1,
    DDPF_ALPHA       = 0x2,
    DDPF_FOURCC      = 0x4,
    DDPF_RGB         = 0x40,
    DDPF_YUV         = 0x200,
    DDPF_LUMINANCE   = 0x20000
};

enum DDSFlags
{
    DDSD_NONE        = 0,
    DDSD_CAPS        = 0x1,
    DDSD_HEIGHT      = 0x2,
    DDSD_WIDTH       = 0x4,
    DDSD_PITCH       = 0x8,
    DDSD_PIXELFORMAT = 0x1000,
    DDSD_MIPMAPCOUNT = 0x20000,
    DDSD_LINEARSIZE  = 0x80000,
    DDSD_DEPTH       = 0x800000
};

enum DDSCaps
{
    DDSCAPS_NONE    = 0,
    DDSCAPS_COMPLEX = 0x8,
    DDSCAPS_MIPMAP  = 0x400000,
    DDSCAPS_TEXTURE = 0x1000
};

enum DDSCaps2
{
    DDSCAPS2_NONE              = 0,
    DDSCAPS2_CUBEMAP           = 0x200,
    DDSCAPS2_CUBEMAP_POSITIVEX = 0x400,
    DDSCAPS2_CUBEMAP_NEGATIVEX = 0x800,
    DDSCAPS2_CUBEMAP_POSITIVEY = 0x1000,
    DDSCAPS2_CUBEMAP_NEGATIVEY = 0x2000,
    DDSCAPS2_CUBEMAP_POSITIVEZ = 0x4000,
    DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x8000,
    DDSCAPS2_VOLUME            = 0x200000
};

struct DDSPF
{
    unsigned int dwSize;
    DDPFFlags    dwFlags;
    unsigned int dwFourCC;
    unsigned int dwRGBBitCount;
    unsigned int dwRBitMask;
    unsigned int dwGBitMask;
    unsigned int dwBBitMask;
    unsigned int dwAlphaBitMask;
};

enum DXGI_FORMAT
{
    DXGI_FORMAT_UNKNOWN         = 0,
    // [...]
    DXGI_FORMAT_BC1_TYPELESS    = 70,
    DXGI_FORMAT_BC1_UNORM,
    DXGI_FORMAT_BC1_UNORM_SRGB,
    DXGI_FORMAT_BC2_TYPELESS,
    DXGI_FORMAT_BC2_UNORM,
    DXGI_FORMAT_BC2_UNORM_SRGB,
    DXGI_FORMAT_BC3_TYPELESS,
    DXGI_FORMAT_BC3_UNORM,
    DXGI_FORMAT_BC3_UNORM_SRGB
    // [...]
};

enum D3D10_RESOURCE_DIMENSION
{
    D3D10_RESOURCE_DIMENSION_UNKNOWN,
    D3D10_RESOURCE_DIMENSION_BUFFER,
    D3D10_RESOURCE_DIMENSION_TEXTURE1D,
    D3D10_RESOURCE_DIMENSION_TEXTURE2D,
    D3D10_RESOURCE_DIMENSION_TEXTURE3D
};

enum DDS_RESOURCE_MISC
{
    DDS_RESOURCE_MISC_NOT_SET     = 0,
    DDS_RESOURCE_MISC_TEXTURECUBE = 0x4
};

//
// DDS standard header
//
struct DDSHeader
{
    unsigned int dwMagic;
    unsigned int dwSize;
    DDSFlags     dwFlags;
    unsigned int dwHeight;
    unsigned int dwWidth;
    unsigned int dwPitchOrLinearSize;
    unsigned int dwDepth;
    unsigned int dwMipMapCount;
    unsigned int dwReserved1[11];
    DDSPF        sPixelFormat;
    DDSCaps      dwCaps1;
    DDSCaps2     dwCaps2;
    unsigned int dwCaps3;
    unsigned int dwCaps4;
    unsigned int dwReserved2;
};

//
// DDS "extended" DX10+ header
//
struct DDSHeader_DXT10
{
    DDSHeader_DXT10()
        : dxgiFormat(DXGI_FORMAT_UNKNOWN)
        , resourceDimension(D3D10_RESOURCE_DIMENSION_UNKNOWN)
        , miscFlag(DDS_RESOURCE_MISC_NOT_SET)
        , arraySize(0)
        , miscFlags2(0)
    {}

    DXGI_FORMAT              dxgiFormat;
    D3D10_RESOURCE_DIMENSION resourceDimension;
    DDS_RESOURCE_MISC        miscFlag;
    unsigned int             arraySize;
    unsigned int             miscFlags2;
};

// make sure we're on the same page
static_assert(sizeof(DDSHeader) == 128 && sizeof(DDSHeader_DXT10) == 20);

constexpr inline unsigned int dds_dxt_fourcc(char c1, char c2, char c3, char c4)
{
    return ((c4 << 24) & 0xff000000) |
           ((c3 << 16) & 0x00ff0000) |
           ((c2 << 8)  & 0x0000ff00) |
           (c1         & 0x000000ff);
}

constexpr inline bool dds_supported_format(const DDSHeader& header, const DDSHeader_DXT10& header10)
{
    bool compressed = ((header.sPixelFormat.dwFlags & DDPF_FOURCC) == DDPF_FOURCC);
    bool extheader = compressed && (header.sPixelFormat.dwFourCC == DDS::dds_dxt_fourcc('D', 'X', '1', '0'));

    unsigned fourCC = header.sPixelFormat.dwFourCC;
    unsigned dxgiFmt = header10.dxgiFormat;

    bool supportedFourCC = (fourCC == DDS::dds_dxt_fourcc('D', 'X', 'T', '1') || fourCC == DDS::dds_dxt_fourcc('D', 'X', 'T', '5'));
    bool supportedDXGI = (dxgiFmt == DXGI_FORMAT_BC1_TYPELESS || dxgiFmt == DXGI_FORMAT_BC1_UNORM || dxgiFmt == DXGI_FORMAT_BC1_UNORM_SRGB ||
                          dxgiFmt == DXGI_FORMAT_BC3_TYPELESS || dxgiFmt == DXGI_FORMAT_BC3_UNORM || dxgiFmt == DXGI_FORMAT_BC3_UNORM_SRGB);

    bool texture2d = !extheader || (extheader && header10.resourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE2D);
    bool texture3d = ((header.dwCaps2 & DDSCAPS2_VOLUME) == DDSCAPS2_VOLUME);
    bool cubemap = ((header.dwCaps2 & DDSCAPS2_CUBEMAP) == DDSCAPS2_CUBEMAP) || (extheader && header10.miscFlag == DDS_RESOURCE_MISC_TEXTURECUBE);

    bool supported = texture2d && !texture3d && !cubemap && compressed && (supportedFourCC || (extheader && supportedDXGI));

    return supported;
}

enum Format { UNSUPPORTED = -1, DXT1, DXT5 };

struct FormatDetails
{
    uint8_t BytesPerBlock;
    GLint Format;
    GLint sRGBFormat;
};

FormatDetails Details[] =
{
    { 8,  GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT }, // DXT1
    { 16, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT }  // DXT5
};

constexpr inline Format dds_compressed_format(const DDSHeader& header, const DDSHeader_DXT10& header10)
{
    Format result = UNSUPPORTED;

    DXGI_FORMAT format = header10.dxgiFormat;
    unsigned int flags = header.sPixelFormat.dwFlags;
    unsigned int fourCC = header.sPixelFormat.dwFourCC;

    if (format != DXGI_FORMAT_UNKNOWN)
    {
        if (format == DXGI_FORMAT_BC1_TYPELESS || format == DXGI_FORMAT_BC1_UNORM || format == DXGI_FORMAT_BC1_UNORM_SRGB)
        {
            result = DXT1;
        }
        else if (format == DXGI_FORMAT_BC3_TYPELESS || format == DXGI_FORMAT_BC3_UNORM || format == DXGI_FORMAT_BC3_UNORM_SRGB)
        {
            result = DXT5;
        }
    }
    else if ((flags & DDPF_FOURCC) == DDPF_FOURCC)
    {
        if (fourCC == DDS::dds_dxt_fourcc('D', 'X', 'T', '1'))
        {
            result = DXT1;
        }
        else if (fourCC == DDS::dds_dxt_fourcc('D', 'X', 'T', '5'))
        {
            result = DXT5;
        }
    }

    return result;
}
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"


Texture::Texture()
	: isValid(false)
	, data(nullptr)
    , isCompressed(false)
{
}

Texture::~Texture()
{
	unload();
}

bool Texture::load(uint8_t* buffer, int len, const vec3& dim, const OnTextureLoaded<Texture>& callback)
{
	isValid = true;
	size = dim;

    dataSize = len;
	data = new uint8_t[dataSize];
	memcpy(data, buffer, dataSize);

	format = GL_RGBA;
	sRGBFormat = GL_SRGB_ALPHA;
	type = GL_UNSIGNED_BYTE;

	onLoadedCallback = callback;
	return true;
}

bool Texture::load(const QString& filename, const OnTextureLoaded<Texture>& callback)
{
	QFile file(filename);
	if (file.open(QIODevice::ReadOnly))
	{
		QByteArray contents = file.readAll();

		int width, height, bpp;
		unsigned char* pixels = nullptr;

		bool isHDR = stbi_is_hdr_from_memory((stbi_uc*)contents.constData(), contents.size());

		pixels = ((isHDR)? (unsigned char*)stbi_loadf_from_memory((stbi_uc*)contents.constData(), contents.size(), &width, &height, &bpp, 4) :
		                                   stbi_load_from_memory((stbi_uc*)contents.constData(), contents.size(), &width, &height, &bpp, 4));

		if (pixels)
		{
			isValid = true;

			size.x = width;
			size.y = height;
			size.z = 1;

			dataSize = width * height * 4 * ((isHDR)? sizeof(float) : sizeof(unsigned char));
			data = new uint8_t[dataSize];
			memcpy(data, pixels, dataSize);

            isCompressed = false;
			format = isHDR? GL_RGBA16F : GL_RGBA;
			sRGBFormat = isHDR? GL_RGBA16F : GL_SRGB_ALPHA;
			type = isHDR? GL_FLOAT : GL_UNSIGNED_BYTE;

			stbi_image_free(pixels);

			onLoadedCallback = callback;
			return true;
		}
		else if (filename.endsWith(".dds"))
		{
            const char* bufPtr = contents.constData();

            DDS::DDSHeader header;
            memcpy(&header, bufPtr, sizeof(DDS::DDSHeader));
            bufPtr += sizeof(DDS::DDSHeader);

            bool extheader  = (header.sPixelFormat.dwFlags & DDS::DDPF_FOURCC) == DDS::DDPF_FOURCC &&
                               header.sPixelFormat.dwFourCC == DDS::dds_dxt_fourcc('D', 'X', '1', '0');

            DDS::DDSHeader_DXT10 header10;
            if (extheader)
            {
                memcpy(&header10, bufPtr, sizeof(DDS::DDSHeader_DXT10));
                bufPtr += sizeof(DDS::DDSHeader_DXT10);
            }         

            if (DDS::dds_supported_format(header, header10))
            {
                DDS::Format dds_format = DDS::dds_compressed_format(header, header10);
                if (format != DDS::UNSUPPORTED)
                {
                    isValid = true;
                    isCompressed = true;

                    size.x = header.dwWidth;
                    size.y = header.dwHeight;
                    size.z = 1;

                    dataSize = max(1u, ((header.dwWidth + 3u) / 4u)) *
                               max(1u, ((header.dwHeight + 3u) / 4u)) *
                               DDS::Details[dds_format].BytesPerBlock;

                    data = new uint8_t[dataSize];
                    memcpy(data, bufPtr, dataSize);

                    format = DDS::Details[dds_format].Format;
                    sRGBFormat = DDS::Details[dds_format].sRGBFormat;

                    onLoadedCallback = callback;
                    return true;
                }   
            }
		}
	}

	return false;
}

void Texture::unload()
{
	delete[] data;
	data = nullptr;

	isValid = false;
}

void Texture::onLoad(const uint8_t* pixelsRGBA) const
{
	if (onLoadedCallback)
	{
		onLoadedCallback(*this, QImage(pixelsRGBA, width(), height(), QImage::Format_RGBA8888));
	}
}
