#ifndef ELEVTILE_H
#define ELEVTILE_H

#include "tile.h"

#define TILE_FILERES 256
#define TILE_ELEVSTRIDE (TILE_FILERES+3)

struct ElevData {
	DWORD width;              // elevation grid width (including padding)
	DWORD height;             // elevation grid height (including padding)
	std::vector<double> data; // elevation grid data [m]
	double dmin, dmax;        // min, max tile elevation [m]
	double dres;              // target elevation resolution [m] (must be 2^n with integer n)
	ElevData();
	ElevData(const ElevData &edata);
	ElevData &operator=(const ElevData &edata);
	ElevData SubTile(const std::pair<DWORD, DWORD> &xrange, const std::pair<DWORD, DWORD> &yrange);
};


class ElevTile : public Tile
{
public:
	ElevTile(const ElevTile &etile);
	static ElevTile *Load(const std::string &root, int lvl, int ilat, int ilng);
	const std::string Layer() const { return std::string("Elev"); }
	double nodeElevation(int ndx, int ndy);
	ElevData &getData() { return m_edata; }
	ElevData &getBaseData() { return m_edataBase; }
	void dataChanged(int exmin = -1, int exmax = -1, int eymin = -1, int eymax = -1) { ExtractImage(exmin, exmax, eymin, eymax); }

protected:
	ElevTile(int lvl, int ilat, int ilng);
	bool Load(const std::string &root);
	void LoadSubset(const std::string &root, ElevTile *tile);
	void ExtractImage(int exmin = -1, int exmax = -1, int eymin = -1, int eymax = -1);

private:
	ElevData m_edata;
	ElevData m_edataBase;
};

#endif // !ELEVTILE_H
