#ifndef TILEBLOCK_H
#define TILEBLOCK_H

#include "tile.h"
#include "elevtile.h"

class TileBlock
{
public:
	TileBlock(int lvl, int ilat0, int ilat1, int ilng0, int ilng1);
	TileBlock(const TileBlock &tileblock);
	int Level() const { return m_lvl; }
	int iLat0() const { return m_ilat0; }
	int iLat1() const { return m_ilat1; }
	int iLng0() const { return m_ilng0; }
	int iLng1() const { return m_ilng1; }
	int nLat() const { return (m_lvl < 4 ? 1 : 1 << (m_lvl - 4)); }
	int nLng() const { return (m_lvl < 4 ? 1 : 1 << (m_lvl - 3)); }

	virtual bool setTile(int ilat, int ilng, const Tile *tile) { return false; }
	virtual bool getTile(int ilat, int ilng, Tile *tile) const { return false; }

protected:
	int m_lvl;
	int m_ilat0, m_ilat1;
	int m_ilng0, m_ilng1;
};


class ElevTileBlock : public TileBlock
{
public:
	static ElevTileBlock *Load(int lvl, int ilat0, int ilat1, int ilng0, int ilng1, ElevDisplayParam &elevDisplayParam);
	ElevTileBlock(int lvl, int ilat0, int ilat1, int ilng0, int ilng1);
	bool setTile(int ilat, int ilng, const Tile *tile);
	bool getTile(int ilat, int ilng, Tile *tile) const;
	ElevData &getData() { return m_edata; }

private:
	ElevData m_edata;
};

#endif // !TILEBLOCK_H