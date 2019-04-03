#include "elevtile.h"
#include "tileblock.h"
#include "elv_io.h"
#include "cmap.h"
#include <iostream>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>

// ==================================================================================

ElevData::ElevData()
{
	width = 0;
	height = 0;
	dmin = 0;
	dmax = 0;
	dres = 1.0;
}

ElevData::ElevData(const ElevData &edata)
	: width(edata.width)
	, height(edata.height)
	, data(edata.data)
	, dmin(edata.dmin)
	, dmax(edata.dmax)
	, dres(edata.dres)
{
}

ElevData &ElevData::operator=(const ElevData &edata)
{
	width = edata.width;
	height = edata.height;
	data = edata.data;
	dmin = edata.dmin;
	dmax = edata.dmax;
	dres = edata.dres;
	return *this;
}

ElevData ElevData::SubTile(const std::pair<DWORD, DWORD> &xrange, const std::pair<DWORD, DWORD> &yrange)
{
	ElevData sub;
	sub.width = xrange.second - xrange.first + 3;
	sub.height = yrange.second - yrange.first + 3;

	sub.data.resize(sub.width * sub.height);

	double *dataptr = data.data();
	double *subptr = sub.data.data();
	int y0 = TILE_FILERES - yrange.second;

	for (int y = 0; y < sub.height; y++) {
		memcpy(subptr + y*sub.width, dataptr + (y + y0) * width + xrange.first, sub.width * sizeof(double));
	}

	sub.dmin = *std::min_element(sub.data.begin(), sub.data.end());
	sub.dmax = *std::max_element(sub.data.begin(), sub.data.end());
	sub.dres = dres;

	return sub;
}

// ==================================================================================

ElevTile::ElevTile(int lvl, int ilat, int ilng, ElevDisplayParam &elevDisplayParam)
	: Tile(lvl, ilat, ilng)
	, m_elevDisplayParam(elevDisplayParam)
{
	lat_subrange.second = 256;
	lng_subrange.second = 256;
	m_modified = false;
}

ElevTile::ElevTile(const ElevTile &etile)
	: Tile(etile)
	, m_elevDisplayParam(etile.m_elevDisplayParam)
	, m_edata(etile.m_edata)
{
	m_modified = false;
}

double ElevTile::nodeElevation(int ndx, int ndy)
{
	int idx = (ndy + 1)*m_edata.width + (ndx + 1);
	return (double)m_edata.data[idx];
}

bool ElevTile::Load(const std::string &root)
{
	// read tile from Elev layer
	char path[1024];
	sprintf(path, "%s/%s/%02d/%06d/%06d.elv", root.c_str(), Layer().c_str(), m_lvl, m_ilat, m_ilng);
	m_edataBase = elvread(path);
	m_edata = m_edataBase;

	if (m_edata.data.size()) {
		// read modifications from Elev_mod layer if present
		sprintf(path, "%s/%s_mod/%02d/%06d/%06d.elv", root.c_str(), Layer().c_str(), m_lvl, m_ilat, m_ilng);
		elvmodread(path, m_edata);
	}
	else {
		// interpolate from ancestor
		LoadSubset(root, this);
	}

	// turn elevation data into an image
	if (m_edata.data.size())
		ExtractImage();

	return m_edata.data.size() > 0;
}

void ElevTile::LoadSubset(const std::string &root, ElevTile *tile)
{
	if (tile->m_sublvl > 1) {
		tile->lat_subrange.first /= 2;
		tile->lat_subrange.second /= 2;
		tile->lng_subrange.first /= 2;
		tile->lng_subrange.second /= 2;
		if (tile->m_subilat & 1) {
			tile->lat_subrange.first += 128;
			tile->lat_subrange.second += 128;
		}
		if (tile->m_subilng & 1) {
			tile->lng_subrange.first += 128;
			tile->lng_subrange.second += 128;
		}
		tile->m_sublvl -= 1;
		tile->m_subilat /= 2;
		tile->m_subilng /= 2;

		char path[1024];
		sprintf(path, "%s/%s/%02d/%06d/%06d.elv", root.c_str(), Layer().c_str(), tile->m_sublvl, tile->m_subilat, tile->m_subilng);
		tile->m_edata = elvread(path);
		if (tile->m_edata.data.size()) {
			sprintf(path, "%s/%s_mod/%02d/%06d/%06d.elv", root.c_str(), Layer().c_str(), tile->m_sublvl, tile->m_subilat, tile->m_subilng);
			elvmodread(path, tile->m_edata);
			tile->m_edata = tile->m_edata.SubTile(tile->lng_subrange, tile->lat_subrange);
		}
		else {
			LoadSubset(root, this);
		}
	}
}

void ElevTile::Save(const std::string &root)
{
	if (m_modified) {
		if (m_lvl == m_sublvl) { // for now, don't support saving ancestor subtiles
			char path[1024];
			sprintf(path, "%s/%s/%02d/%06d/%06d.elv", root.c_str(), Layer().c_str(), m_lvl, m_ilat, m_ilng);
			int nlat = (m_lvl < 4 ? 1 : 1 << (m_lvl - 4));
			int nlng = (m_lvl < 4 ? 1 : 1 << (m_lvl - 3));
			double latmax = (1.0 - (double)m_ilat / (double)nlat) * M_PI - 0.5*M_PI;
			double latmin = latmax - M_PI / nlat;
			double lngmin = (double)m_ilng / (double)nlng * 2.0*M_PI - M_PI;
			double lngmax = lngmin + 2.0*M_PI / nlng;

			elvwrite(path, m_edata, latmin, latmax, lngmin, lngmax);
		}
		m_modified = false;
	}
}

void ElevTile::SaveMod(const std::string &root)
{
	if (m_modified) {
		if (m_lvl == m_sublvl) { // for now, don't support saving ancestor subtiles
			char path[1024];
			sprintf(path, "%s_mod", Layer().c_str());
			ensureLayerDir(root.c_str(), path, m_lvl, m_ilat);
			sprintf(path, "%s/%s_mod/%02d/%06d/%06d.elv", root.c_str(), Layer().c_str(), m_lvl, m_ilat, m_ilng);
			int nlat = (m_lvl < 4 ? 1 : 1 << (m_lvl - 4));
			int nlng = (m_lvl < 4 ? 1 : 1 << (m_lvl - 3));
			double latmax = (1.0 - (double)m_ilat / (double)nlat) * M_PI - 0.5*M_PI;
			double latmin = latmax - M_PI / nlat;
			double lngmin = (double)m_ilng / (double)nlng * 2.0*M_PI - M_PI;
			double lngmax = lngmin + 2.0*M_PI / nlng;

			elvmodwrite(path, m_edata, m_edataBase, latmin, latmax, lngmin, lngmax);
		}
		m_modified = false;
	}
}

void ElevTile::displayParamChanged()
{
	ExtractImage();
}

void ElevTile::MatchNeighbourTiles(const std::string &root)
{
	const double eps = 1e-6;

	int i, xblock, yblock, block_x0, block_y0, ilat, ilng, ilngn;

	int nlat = nLat();
	int nlng = nLng();

	std::vector<ElevTile*> tileGrid(3 * 3);
	for (int i = 0; i < tileGrid.size(); i++)
		tileGrid[i] = 0;

	// load the 3x3 tile neighbourhood
	ElevTileBlock etile3(m_lvl, m_ilat - 1, m_ilat + 2, m_ilng - 1, m_ilng + 2);

	for (yblock = 0; yblock < 3; yblock++) {
		ilat = m_ilat - yblock + 1;
		if (ilat < 0 || ilat >= nlat)
			continue;
		for (xblock = 0; xblock < 3; xblock++) {
			ilng = m_ilng + xblock - 1;
			if (ilat == m_ilat && ilng == m_ilng)
				continue;
			ilngn = (ilng < 0 ? ilng + nlng : ilng >= nlng ? ilng - nlng : ilng);
			ElevTile *etile = ElevTile::Load(root, m_lvl, ilat, ilngn, m_elevDisplayParam);
			if (!etile)
				continue;
			tileGrid[xblock + yblock * 3] = etile;
			etile3.setTile(ilat, ilng, etile);
		}
	}

	// place the modified central tile
	etile3.setTile(m_ilat, m_ilng, this);

	// check for modifications in the neighbours
	for (yblock = 0; yblock < 3; yblock++) {
		for (xblock = 0; xblock < 3; xblock++) {
			ElevTile *etile = tileGrid[xblock + yblock * 3];
			if (etile) {
				ilat = m_ilat - yblock + 1;
				ilng = m_ilng + xblock - 1;
				ilngn = (ilng < 0 ? ilng + nlng : ilng >= nlng ? ilng - nlng : ilng);
				ElevData edata = etile->getData();
				etile3.getTile(ilat, ilng, etile);
				for (i = 0; i < edata.data.size(); i++) {
					if (fabs(edata.data[i] - etile->getData().data[i]) > eps) {
						etile->dataChanged();
						etile->SaveMod(root);
						break;
					}
				}
				delete etile;
			}
		}
	}
}

bool ElevTile::MatchParentTile(const std::string &root, int minlvl) const
{
	const double eps = 1e-6;

	if (m_lvl <= 4 || m_lvl <= minlvl)
		return false;

	int lvl = m_lvl - 1;
	int ilat = m_ilat / 2;
	int ilng = m_ilng / 2;

	ElevTile *etile = ElevTile::Load(root, lvl, ilat, ilng, m_elevDisplayParam);
	if (!etile)
		return false;

	ElevData &edata = etile->getData();
	ElevTileBlock *etile4 = ElevTileBlock::Load(root, m_lvl, ilat * 2 - 1, ilat * 2 + 3, ilng * 2 - 1, ilng * 2 + 3, m_elevDisplayParam);
	ElevData &edata4 = etile4->getData();

	int w4 = edata4.width;
	int h4 = edata4.height;
	int xofs = TILE_FILERES - 1;
	int yofs = TILE_FILERES - 1;
	int ofs = xofs + yofs * w4;
	bool isModified = false;

	for (int y = 0; y < edata.height; y++) {
		for (int x = 0; x < edata.width; x++) {
			int ref = ofs + y * 2 * w4 + x * 2;
			double v = (edata4.data[ref] * 4.0 +
				(edata4.data[ref - 1] + edata4.data[ref + 1] + edata4.data[ref - w4] + edata4.data[ref + w4]) * 2.0 +
				(edata4.data[ref - w4 - 1] + edata4.data[ref - w4 + 1] + edata4.data[ref + w4 - 1] + edata4.data[ref + w4 + 1])) / 16.0;
			if (fabs(edata.data[x + y*edata.width] - v) > eps) {
				edata.data[x + y*edata.width] = v;
				isModified = true;
			}
		}
	}
	delete etile4;

	if (isModified) {
		etile->dataChanged();
		etile->SaveMod(root);
		etile->MatchParentTile(root, minlvl); // recursively propagate changes down the quadtree
	}
	delete etile;

	return isModified;
}

ElevTile *ElevTile::Load(const std::string &root, int lvl, int ilat, int ilng, ElevDisplayParam &elevDisplayParam, const Cmap *cm)
{
	ElevTile *etile = new ElevTile(lvl, ilat, ilng, elevDisplayParam);
	if (!etile->Load(root)) {
		delete etile;
		etile = 0;
	}
	return etile;
}

void ElevTile::dataChanged(int exmin, int exmax, int eymin, int eymax)
{
	ExtractImage(exmin, exmax, eymin, eymax);
	m_modified = true;
}

void ElevTile::ExtractImage(int exmin, int exmax, int eymin, int eymax)
{
	double dmin, dmax;

	img.width = (m_edata.width - 2) * 2 - 2;
	img.height = (m_edata.height - 2) * 2 - 2;
	img.data.resize(img.width * img.height);

	if (m_elevDisplayParam.autoRange) {
		dmin = m_edata.dmin;
		dmax = m_edata.dmax;
	}
	else {
		dmin = m_elevDisplayParam.rangeMin;
		dmax = m_elevDisplayParam.rangeMax;
	}
	double dscale = (dmax > dmin ? 256.0 / (dmax - dmin) : 1.0);

	int imin = (exmin < 0 ? 0 : max(0, (exmin - 1) * 2 - 1));
	int imax = (exmax < 0 ? img.width : min((int)img.width, exmax * 2));
	int jmin = (eymax < 0 ? 0 : max(0, (int)img.height - (eymax - 1) * 2));
	int jmax = (eymin < 0 ? img.height : min((int)img.height, (int)img.height - (eymin - 1) * 2 + 1));

	const Cmap &cm = cmap(m_elevDisplayParam.cmName);
	for (int j = jmin; j < jmax; j++)
		for (int i = imin; i < imax; i++) {
			int ex = (i + 1) / 2 + 1;
			int ey = (img.height - j) / 2 + 1;
			double d = m_edata.data[ex + ey * m_edata.width];
			int v = max(min((int)((d - dmin) * dscale), 255), 0);
			if (v < 0 || v > 255)
				std::cerr << "Problem" << std::endl;
			img.data[i + j * img.width] = (0xff000000 | cm[v]);
		}
}
