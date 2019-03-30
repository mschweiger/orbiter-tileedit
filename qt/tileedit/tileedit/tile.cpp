#include "tile.h"
#include "ddsread.h"
#include <iostream>
#include <algorithm>

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

DXT1Tile::DXT1Tile(int lvl, int ilat, int ilng)
	: Tile(lvl, ilat, ilng)
{
}

void DXT1Tile::LoadDXT1(const std::string &root)
{
	char path[1024];
	sprintf(path, "%s/%s/%02d/%06d/%06d.dds", root.c_str(), Layer().c_str(), m_lvl, m_ilat, m_ilng);
	img = ddsread(path);

	if (img.data.size() == 0) {
		LoadSubset(root, this);
	}
}

void DXT1Tile::LoadSubset(const std::string &root, DXT1Tile *tile)
{
	if (tile->m_sublvl > 1) {
		tile->lat_subrange.first /= 2;
		tile->lat_subrange.second /= 2;
		tile->lng_subrange.first /= 2;
		tile->lng_subrange.second /= 2;
		if (tile->m_subilat & 1) {
			tile->lat_subrange.first += 256;
			tile->lat_subrange.second += 256;
		}
		if (tile->m_subilng & 1) {
			tile->lng_subrange.first += 256;
			tile->lng_subrange.second += 256;
		}
		tile->m_sublvl -= 1;
		tile->m_subilat /= 2;
		tile->m_subilng /= 2;

		char path[1024];
		sprintf(path, "%s/%s/%02d/%06d/%06d.dds", root.c_str(), tile->Layer().c_str(), tile->m_sublvl, tile->m_subilat, tile->m_subilng);
		tile->img = ddsread(path);

		if (tile->img.data.size() == 0) {
			LoadSubset(root, tile);
		}
		else {
			tile->img = tile->img.SubImage(tile->lng_subrange, tile->lat_subrange);
		}
	}
}



SurfTile::SurfTile(int lvl, int ilat, int ilng)
    : DXT1Tile(lvl, ilat, ilng)
{
}

SurfTile *SurfTile::Load(const std::string &root, int lvl, int ilat, int ilng)
{
    SurfTile *stile = new SurfTile(lvl, ilat, ilng);
	stile->LoadDXT1(root);
    return stile;
}

NightlightTile::NightlightTile(const Tile &tile)
	: Tile(tile)
{
}

MaskTile::MaskTile(int lvl, int ilat, int ilng)
	: DXT1Tile(lvl, ilat, ilng)
{
}

std::pair<MaskTile*,NightlightTile*> MaskTile::Load(const std::string &root, int lvl, int ilat, int ilng)
{
	MaskTile *mtile = new MaskTile(lvl, ilat, ilng);
	mtile->LoadDXT1(root);
	NightlightTile *ltile = new NightlightTile(*mtile);

	// separate water mask and night lights
	for (int i = 0; i < mtile->img.data.size(); i++) {
		mtile->img.data[i] |= 0x00FFFFFF;
		ltile->img.data[i] |= 0xFF000000;
	}
	return std::make_pair(mtile, ltile);
}
