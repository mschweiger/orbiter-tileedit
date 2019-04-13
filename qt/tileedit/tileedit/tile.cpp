#include "tile.h"
#include "ddsread.h"
#include <iostream>
#include <algorithm>

int Tile::s_openMode = 0x3;

Tile::Tile(int lvl, int ilat, int ilng)
{
    m_lvl = m_sublvl = lvl;
    m_ilat = m_subilat = ilat;
    m_ilng = m_subilng = ilng;

    int sz = 1 << min(lvl+6, 9);
    lat_subrange = std::make_pair(0, sz);
    lng_subrange = std::make_pair(0, sz);
}

Tile::Tile(const Tile &tile)
{
	m_lvl = tile.m_lvl;
	m_ilat = tile.m_ilat;
	m_ilng = tile.m_ilng;
	m_sublvl = tile.m_sublvl;
	m_subilat = tile.m_subilat;
	m_subilng = tile.m_subilng;
	lat_subrange = tile.lat_subrange;
	lng_subrange = tile.lng_subrange;
	img = tile.img;
}

DWORD Tile::pixelColour(int px, int py) const
{
	return img.data[px + py*img.width];
}

void Tile::setOpenMode(int mode)
{
	s_openMode = mode;
}


DXT1Tile::DXT1Tile(int lvl, int ilat, int ilng)
	: Tile(lvl, ilat, ilng)
{
}

void DXT1Tile::LoadDXT1(const std::string &root, const ZTreeMgr *mgr)
{
	LoadImage(img, root, m_lvl, m_ilat, m_ilng, mgr);

	if (img.data.size() == 0) {
		LoadSubset(root, this, mgr);
	}
}

void DXT1Tile::LoadSubset(const std::string &root, DXT1Tile *tile, const ZTreeMgr *mgr)
{
	if (m_sublvl > 1) {
		lat_subrange.first /= 2;
		lat_subrange.second /= 2;
		lng_subrange.first /= 2;
		lng_subrange.second /= 2;
		if (m_subilat & 1) {
			lat_subrange.first += 256;
			lat_subrange.second += 256;
		}
		if (m_subilng & 1) {
			lng_subrange.first += 256;
			lng_subrange.second += 256;
		}
		m_sublvl -= 1;
		m_subilat /= 2;
		m_subilng /= 2;

		LoadImage(img, root, m_sublvl, m_subilat, m_subilng, mgr);
		if (img.data.size() == 0) {
			LoadSubset(root, tile, mgr);
		}
		else {
			img = img.SubImage(lng_subrange, lat_subrange);
		}
	}
}

void DXT1Tile::LoadImage(Image &im, const std::string &root, int lvl, int ilat, int ilng, const ZTreeMgr *mgr)
{
	if (s_openMode & 0x1) { // try cache
		char path[1024];
		sprintf(path, "%s/%s/%02d/%06d/%06d.dds", root.c_str(), Layer().c_str(), lvl, ilat, ilng);
		im = ddsread(path);
	}
	if (im.data.size() == 0 && s_openMode & 0x2 && mgr) { // try archive
		BYTE *buf;
		DWORD ndata = mgr->ReadData(lvl, ilat, ilng, &buf);
		if (ndata) {
			im = ddsscan(buf, ndata);
			mgr->ReleaseData(buf);
		}
	}
}



const ZTreeMgr *SurfTile::s_treeMgr = 0;

SurfTile::SurfTile(int lvl, int ilat, int ilng)
    : DXT1Tile(lvl, ilat, ilng)
{
}

SurfTile *SurfTile::Load(const std::string &root, int lvl, int ilat, int ilng)
{
    SurfTile *stile = new SurfTile(lvl, ilat, ilng);
	stile->LoadDXT1(root, s_treeMgr);
    return stile;
}

void SurfTile::setTreeMgr(const ZTreeMgr *treeMgr)
{
	s_treeMgr = treeMgr;
}

NightlightTile::NightlightTile(const Tile &tile)
	: Tile(tile)
{
}


const ZTreeMgr *MaskTile::s_treeMgr = 0;

MaskTile::MaskTile(int lvl, int ilat, int ilng)
	: DXT1Tile(lvl, ilat, ilng)
{
}

std::pair<MaskTile*,NightlightTile*> MaskTile::Load(const std::string &root, int lvl, int ilat, int ilng)
{
	MaskTile *mtile = new MaskTile(lvl, ilat, ilng);
	mtile->LoadDXT1(root, s_treeMgr);
	NightlightTile *ltile = new NightlightTile(*mtile);

	// separate water mask and night lights
	for (int i = 0; i < mtile->img.data.size(); i++) {
		mtile->img.data[i] |= 0x00FFFFFF;
		ltile->img.data[i] |= 0xFF000000;
	}
	return std::make_pair(mtile, ltile);
}

void MaskTile::setTreeMgr(const ZTreeMgr *treeMgr)
{
	s_treeMgr = treeMgr;
}
