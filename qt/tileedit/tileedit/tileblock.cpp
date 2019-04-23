#include "tileblock.h"
#include <algorithm>

TileBlock::TileBlock(int lvl, int ilat0, int ilat1, int ilng0, int ilng1)
{
	m_lvl = lvl;
	m_ilat0 = ilat0; m_ilat1 = ilat1;
	m_ilng0 = ilng0; m_ilng1 = ilng1;

	m_nblocklat = m_ilat1 - m_ilat0;
	m_nblocklng = m_ilng1 - m_ilng0;

	m_tile.resize(m_nblocklat * m_nblocklng);
	for (int i = 0; i < m_tile.size(); i++)
		m_tile[i] = 0;
}

TileBlock::TileBlock(const TileBlock &tileblock)
{
	m_lvl = tileblock.m_lvl;
	m_ilat0 = tileblock.m_ilat0;  m_ilat1 = tileblock.m_ilat1;
	m_ilng0 = tileblock.m_ilng0;  m_ilng1 = tileblock.m_ilng1;

	m_nblocklat = tileblock.m_nblocklat;
	m_nblocklng = tileblock.m_nblocklng;

	m_tile.resize(m_nblocklat * m_nblocklng);
	for (int i = 0; i < m_tile.size(); i++)
		m_tile[i] = 0; // the actual assignment must be done by derived classes
}

TileBlock::~TileBlock()
{
	for (int idx = 0; idx < m_tile.size(); idx++)
		if (m_tile[idx])
			delete m_tile[idx];
}

const Tile *TileBlock::getTile(int ilat, int ilng) const
{
	if (ilat < m_ilat0 || ilat >= m_ilat1) return 0;
	if (ilng < m_ilng0 || ilng >= m_ilng1) return 0;

	int idx = (ilat - m_ilat0) * m_nblocklng + (ilng - m_ilng0);
	return m_tile[idx];
}

Tile *TileBlock::getTile(int ilat, int ilng)
{
	if (ilat < m_ilat0 || ilat >= m_ilat1) return 0;
	if (ilng < m_ilng0 || ilng >= m_ilng1) return 0;

	int idx = (ilat - m_ilat0) * m_nblocklng + (ilng - m_ilng0);
	return m_tile[idx];
}



SurfTileBlock::SurfTileBlock(int lvl, int ilat0, int ilat1, int ilng0, int ilng1)
	: TileBlock(lvl, ilat0, ilat1, ilng0, ilng1)
{
}

Tile *SurfTileBlock::copyTile(int ilat, int ilng) const
{
	if (ilat < m_ilat0 || ilat >= m_ilat1) return 0;
	if (ilng < m_ilng0 || ilng >= m_ilng1) return 0;

	int idx = (ilat - m_ilat0) * m_nblocklng + (ilng - m_ilng0);

	if (!m_tile[idx]) return 0;

	SurfTile *stile = static_cast<SurfTile*>(m_tile[idx]);

	return new SurfTile(*stile);
}

bool SurfTileBlock::copyTile(int ilat, int ilng, Tile *tile) const
{
	if (ilat < m_ilat0 || ilat >= m_ilat1) return false;
	if (ilng < m_ilng0 || ilng >= m_ilng1) return false;

	int idx = (ilat - m_ilat0) * m_nblocklng + (ilng - m_ilng0);
	if (!m_tile[idx]) return false;

	SurfTile *stile = static_cast<SurfTile*>(tile);
	if (!stile) return false;

	stile->set(m_tile[idx]);
	return true;
}

SurfTileBlock *SurfTileBlock::Load(int lvl, int ilat0, int ilat1, int ilng0, int ilng1)
{
	SurfTileBlock *stileblock = new SurfTileBlock(lvl, ilat0, ilat1, ilng0, ilng1);

	for (int ilat = ilat0; ilat < ilat1; ilat++) {
		for (int ilng = ilng0; ilng < ilng1; ilng++) {
			int idx = (ilat - ilat0)*stileblock->m_nblocklng + (ilng - ilng0);
			stileblock->m_tile[idx] = SurfTile::Load(lvl, ilat, ilng);
		}
	}
	return stileblock;
}


NightlightTileBlock::NightlightTileBlock(int lvl, int ilat0, int ilat1, int ilng0, int ilng1)
	: TileBlock(lvl, ilat0, ilat1, ilng0, ilng1)
{
}

Tile *NightlightTileBlock::copyTile(int ilat, int ilng) const
{
	if (ilat < m_ilat0 || ilat >= m_ilat1) return 0;
	if (ilng < m_ilng0 || ilng >= m_ilng1) return 0;

	int idx = (ilat - m_ilat0) * m_nblocklng + (ilng - m_ilng0);

	if (!m_tile[idx]) return 0;

	NightlightTile *ltile = static_cast<NightlightTile*>(m_tile[idx]);

	return new NightlightTile(*ltile);
}

bool NightlightTileBlock::copyTile(int ilat, int ilng, Tile *tile) const
{
	if (ilat < m_ilat0 || ilat >= m_ilat1) return false;
	if (ilng < m_ilng0 || ilng >= m_ilng1) return false;

	int idx = (ilat - m_ilat0) * m_nblocklng + (ilng - m_ilng0);
	if (!m_tile[idx]) return false;

	NightlightTile *ltile = static_cast<NightlightTile*>(tile);
	if (!ltile) return false;

	ltile->set(m_tile[idx]);
	return true;
}


MaskTileBlock::MaskTileBlock(int lvl, int ilat0, int ilat1, int ilng0, int ilng1)
	: TileBlock(lvl, ilat0, ilat1, ilng0, ilng1)
{
}

Tile *MaskTileBlock::copyTile(int ilat, int ilng) const
{
	if (ilat < m_ilat0 || ilat >= m_ilat1) return 0;
	if (ilng < m_ilng0 || ilng >= m_ilng1) return 0;

	int idx = (ilat - m_ilat0) * m_nblocklng + (ilng - m_ilng0);

	if (!m_tile[idx]) return 0;

	MaskTile *mtile = static_cast<MaskTile*>(m_tile[idx]);

	return new MaskTile(*mtile);
}

bool MaskTileBlock::copyTile(int ilat, int ilng, Tile *tile) const
{
	if (ilat < m_ilat0 || ilat >= m_ilat1) return false;
	if (ilng < m_ilng0 || ilng >= m_ilng1) return false;

	int idx = (ilat - m_ilat0) * m_nblocklng + (ilng - m_ilng0);
	if (!m_tile[idx]) return false;

	MaskTile *mtile = static_cast<MaskTile*>(tile);
	if (!mtile) return false;

	mtile->set(m_tile[idx]);
	return true;
}

std::pair<MaskTileBlock*, NightlightTileBlock*> MaskTileBlock::Load(int lvl, int ilat0, int ilat1, int ilng0, int ilng1)
{
	MaskTileBlock *mtileblock = new MaskTileBlock(lvl, ilat0, ilat1, ilng0, ilng1);
	NightlightTileBlock *ltileblock = new NightlightTileBlock(lvl, ilat0, ilat1, ilng0, ilng1);

	for (int ilat = ilat0; ilat < ilat1; ilat++) {
		for (int ilng = ilng0; ilng < ilng1; ilng++) {
			int idx = (ilat - ilat0)*mtileblock->m_nblocklng + (ilng - ilng0);
			std::pair<MaskTile*, NightlightTile*> mltile = MaskTile::Load(lvl, ilat, ilng);
			mtileblock->m_tile[idx] = mltile.first;
			ltileblock->m_tile[idx] = mltile.second;
		}
	}
	return std::make_pair(mtileblock, ltileblock);
}



ElevTileBlock::ElevTileBlock(int lvl, int ilat0, int ilat1, int ilng0, int ilng1)
	: TileBlock(lvl, ilat0, ilat1, ilng0, ilng1)
{
	m_edata.width = (ilng1 - ilng0) * TILE_FILERES + 3;
	m_edata.height = (ilat1 - ilat0) * TILE_FILERES + 3;
	m_edata.data.resize(m_edata.width * m_edata.height);
}

ElevTileBlock::ElevTileBlock(const ElevTileBlock &etileblock)
	: TileBlock(etileblock)
{
	for (int ilat = m_ilat0; ilat < m_ilat1; ilat++)
		for (int ilng = m_ilng0; ilng < m_ilng1; ilng++) {
			int idx = (ilat - m_ilat0)*m_nblocklng + (ilng - m_ilng0);
			m_tile[idx] = etileblock.copyTile(ilat, ilng);
		}

	m_edata = etileblock.m_edata;
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

Tile *ElevTileBlock::copyTile(int ilat, int ilng) const
{
	if (ilat < m_ilat0 || ilat >= m_ilat1) return 0;
	if (ilng < m_ilng0 || ilng >= m_ilng1) return 0;

	int idx = (ilat - m_ilat0) * m_nblocklng + (ilng - m_ilng0);

	if (!m_tile[idx]) return 0;

	ElevTile *etile = static_cast<ElevTile*>(m_tile[idx]);

	return new ElevTile(*etile);
}

bool ElevTileBlock::copyTile(int ilat, int ilng, Tile *tile) const
{
	if (ilat < m_ilat0 || ilat >= m_ilat1) return false;
	if (ilng < m_ilng0 || ilng >= m_ilng1) return false;

	int idx = (ilat - m_ilat0) * m_nblocklng + (ilng - m_ilng0);
	if (!m_tile[idx]) return false;
	
	ElevTile *etile = static_cast<ElevTile*>(tile);
	if (!etile) return false;

	etile->set(m_tile[idx]);
	return true;
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

#ifdef UNDEF
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
#endif