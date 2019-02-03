#ifndef ELVREAD_H
#define ELVREAD_H

struct ElevData {
	DWORD width;
	DWORD height;
	std::vector<INT16> data;
	ElevData SubTile(const std::pair<DWORD, DWORD> &xrange, const std::pair<DWORD, DWORD> &yrange);
};

ElevData elvread(const char *fname);
void elvmodread(const char *fname, ElevData &edata);

#endif // !ELVREAD_H
