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

int dxtread_png(const char *fname, const SurfPatchMetaInfo &meta, Image &sdata)
{
	int res = 0;
	png_image image;
	memset(&image, 0, sizeof(png_image));
	image.version = PNG_IMAGE_VERSION;
	image.opaque = NULL;
	if (png_image_begin_read_from_file(&image, fname)) {
		image.format = PNG_FORMAT_RGBA;

		int w = sdata.width;
		int h = sdata.height;
		if (image.width == w && image.height == h) {

			int n = w*h;
			unsigned char *buf = new unsigned char[n * 4];
			png_image_finish_read(&image, NULL, buf, w * 4, NULL);

			int idx = 0;
			for (int ih = 0; ih < h; ih++) {
				for (int iw = 0; iw < w; iw++) {
					DWORD v;
					if (!meta.alphaBlend || buf[idx + 3] == 0xff) {
						v = 0xff000000 | (buf[idx + 2] << 16) | (buf[idx + 1] << 8) | buf[idx];
						sdata.data[iw + ih*w] = v;
					} 
					else if (buf[idx + 3]) {
						v = 0xff000000;
						DWORD s = buf[idx + 3];
						DWORD ch1 = (DWORD)buf[idx + 2];
						DWORD ch2 = (sdata.data[iw + ih*w] >> 16) & 0xFF;
						DWORD ch = (ch1 * s + ch2 * (255 - s)) / 255;
						v |= ch << 16;
						ch1 = (DWORD)buf[idx + 1];
						ch2 = (sdata.data[iw + ih*w] >> 8) & 0xff;
						ch = (ch1 * s + ch2 * (255 - s)) / 255;
						v |= ch << 8;
						ch1 = (DWORD)buf[idx];
						ch2 = (sdata.data[iw + ih*w] & 0xff);
						ch = (ch1 * s + ch2 * (255 - s)) / 255;
						v |= ch;
						sdata.data[iw + ih*w] = v;
					}
					idx += 4;
				}
			}
			delete[]buf;
		}
		else {
			res = -2;
		}
	}
	else {
		res = -1;
	}
	png_image_free(&image);
	return res;
}