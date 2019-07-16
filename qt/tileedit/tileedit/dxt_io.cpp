#include "dxt_io.h"
#include <png.h>
#include <libdxt.h>

struct DDS_PIXELFORMAT {
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwFourCC;
	DWORD dwRGBBitCount;
	DWORD dwRBitMask;
	DWORD dwGBitMask;
	DWORD dwBBitMask;
	DWORD dwABitMask;
};

struct DDS_HEADER{
	DWORD           dwSize;
	DWORD           dwFlags;
	DWORD           dwHeight;
	DWORD           dwWidth;
	DWORD           dwPitchOrLinearSize;
	DWORD           dwDepth;
	DWORD           dwMipMapCount;
	DWORD           dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	DWORD           dwCaps;
	DWORD           dwCaps2;
	DWORD           dwCaps3;
	DWORD           dwCaps4;
	DWORD           dwReserved2;
};

void setdxt1header(const Image &idata, DDS_HEADER &hdr)
{
	const char fourcc[4] = { 'D', 'X', 'T', '1' };

	memset(&hdr, 0, sizeof(DDS_HEADER));
	hdr.dwSize = sizeof(DDS_HEADER);
	hdr.dwFlags = 0x00081007;
	hdr.dwHeight = idata.height;
	hdr.dwWidth = idata.width;
	hdr.dwPitchOrLinearSize = idata.height * idata.width / 2;
	hdr.ddspf.dwSize = sizeof(DDS_PIXELFORMAT);
	hdr.ddspf.dwFlags = 0x4;
	hdr.ddspf.dwFourCC = *(DWORD*)fourcc;
	hdr.dwCaps = 0x1000;
}

void dxt1write(const char *fname, const Image &idata)
{
	const char magic[4] = { 'D', 'D', 'S', ' ' };
	int format = FORMAT_DXT1;
	byte *dxt1 = new byte[idata.width * idata.height * 4];
	int n = CompressDXT((const byte*)idata.data.data(), dxt1, idata.width, idata.height, format);

	DDS_HEADER hdr;
	setdxt1header(idata, hdr);
	FILE *f = fopen(fname, "wb");
	fwrite(magic, 1, 4, f);
	fwrite(&hdr, sizeof(DDS_HEADER), 1, f);
	fwrite(dxt1, n, 1, f);
	fclose(f);
	delete[]dxt1;
}

bool dxtread_png(const char *fname, const SurfPatchMetaInfo &meta, Image &sdata)
{
	bool ok = false;
	png_image image;
	memset(&image, 0, sizeof(png_image));
	image.version = PNG_IMAGE_VERSION;
	image.opaque = NULL;
	if (png_image_begin_read_from_file(&image, fname)) {
		image.format = PNG_FORMAT_RGB;

		int nblock_x = meta.ilng1 - meta.ilng0;
		int nblock_y = meta.ilat1 - meta.ilat0;
		int w = nblock_x * TILE_SURFSTRIDE;
		int h = nblock_y * TILE_SURFSTRIDE;
		if (image.width == w || image.height == h) {

			int n = w*h;
			unsigned char *buf = new unsigned char[n * 3];
			png_image_finish_read(&image, NULL, buf, w * 3, NULL);

			int idx = 0;
			for (int ih = 0; ih < h; ih++) {
				for (int iw = 0; iw < w; iw++) {
					DWORD v = 0xff000000 | (buf[idx + 2] << 16) | (buf[idx + 1] << 8) | buf[idx];
					sdata.data[iw + ih*w] = v;
					idx += 3;
				}
			}
			delete[]buf;
			ok = true;
		}
	}
	png_image_free(&image);
	return ok;
}