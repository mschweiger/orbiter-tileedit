#ifndef DXT_IO_H
#define DXT_IO_H

#include "tile.h"

struct SurfPatchMetaInfo
{
	int lvl;
	int ilat0, ilat1, ilng0, ilng1;
	double latmin, latmax, lngmin, lngmax;
	std::vector<std::pair<int, int> > missing;
};

void dxt1write(const char *fname, const Image &idata);

bool dxtread_png(const char *fname, const SurfPatchMetaInfo &meta, Image &sdata);

#endif // !DXT_IO_H