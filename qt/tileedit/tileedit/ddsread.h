#ifndef DDSREAD_H
#define DDSREAD_H

struct Image {
    std::vector<DWORD> data;
    DWORD width;
    DWORD height;

	Image() { width = height = 0; }
    Image SubImage(const std::pair<DWORD,DWORD> &xrange, const std::pair<DWORD,DWORD> &yrange);
};

Image ddsread(const char *fname);
Image ddsscan(const BYTE *data, int ndata);

#endif // DDSREAD_H
