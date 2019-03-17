#ifndef ELV_IO_H
#define ELV_IO_H

#include "elevtile.h"

ElevData elvread(const char *fname);
void elvmodread(const char *fname, ElevData &edata);

void elvwrite(const char *fname, const ElevData &edata, double latmin, double latmax, double lngmin, double lngmax);
void elvmodwrite(const char *fname, const ElevData &edata, const ElevData &ebasedata);

#endif // !ELV_IO_H
