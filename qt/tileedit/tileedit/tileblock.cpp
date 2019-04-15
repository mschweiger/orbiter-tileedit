#include "tileblock.h"
#include <algorithm>

TileBlock::TileBlock(int lvl, int ilat0, int ilat1, int ilng0, int ilng1)
{
	m_lvl = lvl;
	m_ilat0 = ilat0; m_ilat1 = ilat1;
	m_ilng0 = ilng0; m_ilng1 = ilng1;
}

TileBlock::TileBlock(const TileBlock &tileblock)
{
	m_lvl = tileblock.m_lvl;
	m_ilat0 = tileblock.m_ilat0;  m_ilat1 = tileblock.m_ilat1;
	m_ilng0 = tileblock.m_ilng0;  m_ilng1 = tileblock.m_ilng1;
}



ElevTileBlock::ElevTileBlock(int lvl, int ilat0, int ilat1, int ilng0, int ilng1)
	: TileBlock(lvl, ilat0, ilat1, ilng0, ilng1)
{
	m_edata.width = (ilng1 - ilng0) * TILE_FILERES + 3;
	m_edata.height = (ilat1 - ilat0) * TILE_FILERES + 3;
	m_edata.data.resize(m_edata.width * m_edata.height);
}

ElevTileBlock *ElevTileBlock::Load(int lvl, int ilat0, int ilat1, int ilng0, int ilng1, ElevDisplayParam &elevDisplayParam)
{
	ElevTileBlock *tileblock = new ElevTileBlock(lvl, ilat0, ilat1, ilng0, ilng1);
	int nlat = tileblock->nLat();
	int nlng = tileblock->nLng();
	for (int ilat = ilat0; ilat < ilat1; ilat++) {
		if (ilat < 0 || ilat >= nlat) continue;
		for (int ilng = ilng0; ilng < ilng1; ilng++) {
			int ilngn = ilng;
			while (ilngn < 0) ilngn += nlng;
			while (ilngn >= nlng) ilngn -= nlng;
			ElevTile *tile = ElevTile::Load(lvl, ilat, ilngn, elevDisplayParam);
			tileblock->setTile(ilat, ilng, tile);
			delete tile;
		}
	}
	return tileblock;
}

bool ElevTileBlock::setTile(int ilat, int ilng, const Tile *tile)
{
	if (ilat < m_ilat0 || ilat >= m_ilat1) return false;
	if (ilng < m_ilng0 || ilng >= m_ilng1) return false;

	const ElevTile *etile = static_cast<const ElevTile*>(tile);

	int nlat = nLat();
	int nlng = nLng();

	int xblock = ilng - m_ilng0;
	int yblock = m_ilat1 - 1 - ilat;

	int block_x0 = xblock * TILE_FILERES;
	int block_y0 = yblock * TILE_FILERES;

	int x0 = (xblock == 0 ? 0 : 1);
	int x1 = (xblock == m_ilng1 - m_ilng0 - 1 ? TILE_ELEVSTRIDE : TILE_ELEVSTRIDE - 1);
	int y0 = (yblock == 0 || ilat == nlat - 1 ? 0 : 1);
	int y1 = (yblock == m_ilat1 - m_ilat0 - 1 || ilat == 0 ? TILE_ELEVSTRIDE : TILE_ELEVSTRIDE - 1);

	for (int y = y0; y < y1; y++) {
		for (int x = x0; x < x1; x++) {
			m_edata.data[(block_y0 + y) * m_edata.width + (block_x0 + x)] =
				etile->getData().data[y*TILE_ELEVSTRIDE + x];
		}
	}
	return true;
}

bool ElevTileBlock::getTile(int ilat, int ilng, Tile *tile) const
{
	ElevTile *etile = static_cast<ElevTile*>(tile);

	int nlat = nLat();
	int nlng = nLng();

	if (ilat < 0 || ilat >= nlat) return false;
	int ilngn = ilng;
	while (ilngn < 0) ilngn += nlng;
	while (ilngn >= nlng) ilngn -= nlng;

	ElevData &edata = etile->getData();

	int xblock = ilng - m_ilng0;
	int yblock = m_ilat1 - 1 - ilat;
	int block_x0 = xblock * TILE_FILERES;
	int block_y0 = yblock * TILE_FILERES;

	for (int y = 0; y < TILE_ELEVSTRIDE; y++) {
		for (int x = 0; x < TILE_ELEVSTRIDE; x++) {
			edata.data[y*TILE_ELEVSTRIDE + x] =
				m_edata.data[(block_y0 + y) * m_edata.width + (block_x0 + x)];
		}
	}

	edata.dmin = *std::min_element(edata.data.begin(), edata.data.end());
	edata.dmax = *std::max_element(edata.data.begin(), edata.data.end());
	etile->dataChanged();

	return true;
}