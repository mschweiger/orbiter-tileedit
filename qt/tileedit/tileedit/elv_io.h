#ifndef ELV_IO_H
#define ELV_IO_H

#include "elevtile.h"

ElevData elvread(const char *fname);
bool elvmodread(const char *fname, ElevData &edata);

ElevData elvscan(const BYTE *data, int ndata);
bool elvmodscan(const BYTE *data, int ndata, ElevData &edata);

void elvwrite(const char *fname, const ElevData &edata, double latmin, double latmax, double lngmin, double lngmax);
void elvmodwrite(const char *fname, const ElevData &edata, const ElevData &ebasedata, double latmin, double latmax, double lngmin, double lngmax);

void elvwrite_png(const char *fname, const ElevData &edata, double *data_scale, double *data_offset);

void ensureLayerDir(const char *rootDir, const char *layer, int lvl, int ilat);

#endif // !ELV_IO_H
