#ifndef TILE_H
#define TILE_H

#include <windows.h>
#include <vector>
#include "ddsread.h"

class Tile
{
public:
    Tile(int lvl, int ilat, int ilng);
	Tile(const Tile &tile);
    int Level() const { return m_lvl; }
    int iLat() const { return m_ilat; }
    int iLng() const { return m_ilng; }
	int subLevel() const { return m_sublvl; }
	int subiLat() const { return m_subilat; }
	int subiLng() const { return m_subilng; }
	int nLat() const { return (m_lvl < 4 ? 1 : 1 << (m_lvl - 4)); }
	int nLng() const { return (m_lvl < 4 ? 1 : 1 << (m_lvl - 3)); }

    const Image &getImage() const { return img; }
	void setImage(const Image &im) { img = im; }
	DWORD pixelColour(int px, int py) const;
	virtual const std::string Layer() const = 0;

protected:
    int m_lvl;
    int m_ilat;
    int m_ilng;
    int m_sublvl;
    int m_subilat;
    int m_subilng;
    std::pair<DWORD, DWORD> lat_subrange;
    std::pair<DWORD, DWORD> lng_subrange;
    Image img;
};

class DXT1Tile: public Tile
{
public:
	DXT1Tile(int lvl, int ilat, int ilng);

protected:
	virtual void LoadDXT1(const std::string &root);
	static void LoadSubset(const std::string &root, DXT1Tile *tile);
};

class SurfTile: public DXT1Tile
{
public:
    static SurfTile *Load(const std::string &root, int lvl, int ilat, int ilng);
	const std::string Layer() const { return std::string("Surf"); }

protected:
    SurfTile(int lvl, int ilat, int ilng);
};

class NightlightTile : public Tile
{
	friend class MaskTile;

public:
	const std::string Layer() const { return std::string("Mask"); }

protected:
	NightlightTile(const Tile &tile);
};

class MaskTile : public DXT1Tile
{
public:
	static std::pair<MaskTile*, NightlightTile*> Load(const std::string &root, int lvl, int ilat, int ilng);
	const std::string Layer() const { return std::string("Mask"); }

protected:
	MaskTile(int lvl, int ilat, int ilng);
};

#endif // TILE_H
