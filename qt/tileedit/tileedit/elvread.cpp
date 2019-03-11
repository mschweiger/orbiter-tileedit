#include <windows.h>
#include <vector>
#include <algorithm>
#include "elvread.h"

#define TILE_FILERES 256
#define TILE_ELEVSTRIDE (TILE_FILERES+3)

#pragma pack(push,1)

struct ELEVFILEHEADER { // file header for patch elevation data file
	char id[4];            // ID string + version ('E','L','E',1)
	int hdrsize;           // header size (76 expected)
	int dtype;             // data format (0=flat, no data block; 8=uint8; -8=int8; 16=uint16; -16=int16)
	int xgrd, ygrd;         // data grid size (259 x 259 expected)
	int xpad, ypad;         // horizontal, vertical padding width (1, 1 expected)
	double scale;          // data scaling factor (1.0 expected)
	double offset;         // data offset (elevation = raw value * scale + offset)
	double latmin, latmax; // latitude range [rad]
	double lngmin, lngmax; // longitude range [rad]
	double emin, emax, emean; // min, max, mean elevation [m]
};

#pragma pack(pop)

ElevData::ElevData()
{
	width = 0;
	height = 0;
	dmin = 0;
	dmax = 0;
}

ElevData::ElevData(const ElevData &edata)
	: width(edata.width)
	, height(edata.height)
	, data(edata.data)
	, dmin(edata.dmin)
	, dmax(edata.dmax)
{
}

ElevData &ElevData::operator=(const ElevData &edata)
{
	width = edata.width;
	height = edata.height;
	data = edata.data;
	dmin = edata.dmin;
	dmax = edata.dmax;
	return *this;
}

ElevData ElevData::SubTile(const std::pair<DWORD, DWORD> &xrange, const std::pair<DWORD, DWORD> &yrange)
{
	ElevData sub;
	sub.width = xrange.second - xrange.first + 3;
	sub.height = yrange.second - yrange.first + 3;

	sub.data.resize(sub.width * sub.height);

	INT16 *dataptr = data.data();
	INT16 *subptr = sub.data.data();
	int y0 = TILE_FILERES - yrange.second;

	for (int y = 0; y < sub.height; y++) {
		memcpy(subptr + y*sub.width, dataptr + (y + y0) * width + xrange.first, sub.width * sizeof(INT16));
	}
	return sub;
}

ElevData elvread(const char *fname)
{
	const int ndat = TILE_ELEVSTRIDE*TILE_ELEVSTRIDE;
	ElevData edata;
	ELEVFILEHEADER hdr;
	double scale, offset;
	int i;

	FILE *f = fopen(fname, "rb");
	if (!f)
		return edata;

	int res = fread(&hdr, sizeof(ELEVFILEHEADER), 1, f);
	if (res != 1 || strncmp(hdr.id, "ELE\01", 4))
		return edata;

	if (hdr.hdrsize != sizeof(ELEVFILEHEADER)) {
		fseek(f, hdr.hdrsize, SEEK_SET);
	}
	scale = hdr.scale;
	offset = hdr.offset;
	edata.data.resize(ndat);
	edata.width = TILE_ELEVSTRIDE;
	edata.height = TILE_ELEVSTRIDE;
	INT16 *e = edata.data.data();

	switch (hdr.dtype) {
	case 0:
		for (i = 0; i < ndat; i++) e[i] = 0;
		break;
	case 8: {
		UINT8 *tmp = new UINT8[ndat];
		fread(tmp, sizeof(UINT8), ndat, f);
		for (i = 0; i < ndat; i++)
			e[i] = (INT16)tmp[i];
		delete[]tmp;
		}
		break;
	case -16:
		fread(e, sizeof(INT16), ndat, f);
		break;
	}
	fclose(f);

	if (offset) {
		INT16 ofs16 = (INT16)offset;
		for (i = 0; i < ndat; i++)
			e[i] += ofs16;
	}
	edata.dmin = *std::min_element(edata.data.begin(), edata.data.end());
	edata.dmax = *std::max_element(edata.data.begin(), edata.data.end());
	return edata;
}

void elvmodread(const char *fname, ElevData &edata)
{
	const int ndat = TILE_ELEVSTRIDE*TILE_ELEVSTRIDE;
	ELEVFILEHEADER hdr;
	int i;

	FILE *f = fopen(fname, "rb");
	if (!f)
		return;

	int res = fread(&hdr, sizeof(ELEVFILEHEADER), 1, f);
	if (res != 1 || strncmp(hdr.id, "ELE\01", 4))
		return;

	if (hdr.hdrsize != sizeof(ELEVFILEHEADER)) {
		fseek(f, hdr.hdrsize, SEEK_SET);
	}
	INT16 *e = edata.data.data();
	INT16 ofs16 = (INT16)hdr.offset;

	switch (hdr.dtype) {
	case 0:
		for (i = 0; i < ndat; i++) e[i] = ofs16;
		break;
	case 8: {
		UINT8 *tmp = new UINT8[ndat];
		fread(tmp, sizeof(UINT8), ndat, f);
		for (i = 0; i < ndat; i++)
			if (tmp[i] != UCHAR_MAX)
				e[i] = (INT16)tmp[i] + ofs16;
		delete[]tmp;
		}
		break;
	case -16: {
		INT16 *tmp = new INT16[ndat];
		fread(tmp, sizeof(INT16), ndat, f);
		for (i = 0; i < ndat; i++)
			if (tmp[i] != SHRT_MAX)
				e[i] = tmp[i] + ofs16;
		delete[]tmp;
		}
		break;
	}
	fclose(f);
	edata.dmin = *std::min_element(edata.data.begin(), edata.data.end());
	edata.dmax = *std::max_element(edata.data.begin(), edata.data.end());
}