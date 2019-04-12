#ifndef ELEVTILE_H
#define ELEVTILE_H

#include "tile.h"
#include "cmap.h"

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

struct ElevDisplayParam {
	CmapName cmName;
	bool useWaterMask;
	bool autoRange;
	double rangeMin;
	double rangeMax;

	ElevDisplayParam() {
		cmName = CMAP_GREY;
		useWaterMask = false;
		autoRange = true;
		rangeMin = 0.0;
		rangeMax = 1000.0;
	}
};


class ElevTile : public Tile {
	friend class ElevTileBlock;

public:
	ElevTile(const ElevTile &etile);
	static ElevTile *Load(const std::string &root, int lvl, int ilat, int ilng, ElevDisplayParam &elevDisplayParam, const Cmap *cm = 0);
	static void setTreeMgr(const ZTreeMgr *mgr, const ZTreeMgr *modMgr = 0);
	const std::string Layer() const { return std::string("Elev"); }
	double nodeElevation(int ndx, int ndy);
	ElevData &getData() { return m_edata; }
	const ElevData &getData() const { return m_edata; }
	ElevData &getBaseData() { return m_edataBase; }
	void displayParamChanged();
	bool isModified() const { return m_modified; }
	void dataChanged(int exmin = -1, int exmax = -1, int eymin = -1, int eymax = -1);
	void Save(const std::string &root);
	void SaveMod(const std::string &root);
	void MatchNeighbourTiles(const std::string &root);
	bool MatchParentTile(const std::string &root, int minlvl) const;
	void setWaterMask(const MaskTile *mtile);

protected:
	ElevTile(int lvl, int ilat, int ilng, ElevDisplayParam &elevDisplayParam);
	bool Load(const std::string &root);
	void LoadSubset(const std::string &root);
	void LoadData(ElevData &edata, const std::string &root, int lvl, int ilat, int ilng);
	void LoadModData(ElevData &edata, const std::string &root, int lvl, int ilat, int ilng);
	void ExtractImage(int exmin = -1, int exmax = -1, int eymin = -1, int eymax = -1);

private:
	ElevData m_edata;
	ElevData m_edataBase;
	bool m_modified;
	ElevDisplayParam &m_elevDisplayParam;
	std::vector<bool> m_waterMask;
	static const ZTreeMgr *s_treeMgr;
	static const ZTreeMgr *s_treeModMgr;
};

#endif // !ELEVTILE_H
