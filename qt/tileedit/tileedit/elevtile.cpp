#include "elevtile.h"
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

ElevTile::ElevTile(int lvl, int ilat, int ilng)
	: Tile(lvl, ilat, ilng)
{
	lat_subrange.second = 256;
	lng_subrange.second = 256;
	m_modified = false;
}

ElevTile::ElevTile(const ElevTile &etile)
	: Tile(etile)
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

void ElevTile::MatchNeighbourTiles(const std::string &root)
{
	int size_3x3 = TILE_FILERES * 3 + 3;
	std::vector<double> edata_3x3(size_3x3 * size_3x3 * 3);

	std::vector<ElevTile*> tileGrid(3 * 3);
	for (int i = 0; i < tileGrid.size(); i++)
		tileGrid[i] = 0;

	int nlat = (m_lvl < 4 ? 1 : 1 << (m_lvl - 4));
	int nlng = (m_lvl < 4 ? 1 : 1 << (m_lvl - 3));

	int i, j, i0, i1, j0, j1, xblock, yblock, ilat, ilng, block_x0, block_y0;
	bool isModified;

	// fill the 3x3 tile neighbourhood
	for (yblock = 0; yblock < 3; yblock++) {
		ilat = m_ilat - yblock + 1;
		if (ilat < 0 || ilat >= nlat)
			continue;
		block_y0 = yblock*TILE_FILERES;
		for (xblock = 0; xblock < 3; xblock++) {
			ilng = m_ilng + xblock - 1;
			if (ilng < 0) ilng = nlng - 1;
			else if (ilng >= nlng) ilng = 0;
			if (ilat == m_ilat && ilng == m_ilng)
				continue;
			block_x0 = xblock*TILE_FILERES;
			ElevTile *etile = ElevTile::Load(root, m_lvl, ilat, ilng);
			if (etile) {
				tileGrid[xblock + yblock * 3] = etile;
				j0 = (yblock == 0 || ilat == nlat - 1 ? 0 : 1);
				j1 = (yblock == 2 || ilat == 0 ? TILE_ELEVSTRIDE : TILE_ELEVSTRIDE - 1);
				i0 = (xblock == 0 ? 0 : 1);
				i1 = (xblock == 2 ? TILE_ELEVSTRIDE : TILE_ELEVSTRIDE - 1);
				for (j = j0; j < j1; j++) {
					for (i = i0; i < i1; i++) {
						edata_3x3[(block_y0 + j) * size_3x3 + (block_x0 + i)] = etile->getData().data[j*TILE_ELEVSTRIDE + i];
					}
				}
			}
		}
	}

	// place the modified centre tile, but strip padding
	block_y0 = TILE_FILERES;
	block_x0 = TILE_FILERES;
	j0 = (ilat == nlat - 1 ? 0 : 1);
	j1 = (ilat == 0 ? TILE_ELEVSTRIDE : TILE_ELEVSTRIDE - 1);
	i0 = 1;
	i1 = TILE_ELEVSTRIDE - 1;
	for (j = j0; j < j1; j++) {
		for (i = i0; i < i1; i++)
			edata_3x3[(block_y0+j) * size_3x3 + (block_x0 + i)] = m_edata.data[j*TILE_ELEVSTRIDE + i];
	}

	// check for modifications in the neighbours
	for (int yblock = 0; yblock < 3; yblock++) {
		for (int xblock = 0; xblock < 3; xblock++) {
			ElevTile *etile = tileGrid[yblock * 3 + xblock];
			if (etile) {
				isModified = false;
				int ilat = m_ilat - yblock + 1;
				int ilng = m_ilng + xblock - 1;
				if (ilng < 0) ilng = nlng - 1;
				else if (ilng >= nlng) ilng = 0;
				block_y0 = yblock*TILE_FILERES;
				block_x0 = xblock*TILE_FILERES;
				for (j = 0; j < TILE_ELEVSTRIDE; j++) {
					for (i = 0; i < TILE_ELEVSTRIDE; i++) {
						double v = edata_3x3[(block_y0 + j)*size_3x3 + (block_x0 + i)];
						double vold = etile->getData().data[j*TILE_ELEVSTRIDE + i];
						if (fabs(v - vold) > 1e-8) {
							etile->getData().data[j*TILE_ELEVSTRIDE + i] = v;
							isModified = true;
						}
					}
				}
				if (isModified) {
					if (etile->getData().dres > m_edata.dres)
						etile->getData().dres = m_edata.dres; // make sure we can represent the overlap with the same resolution
					etile->dataChanged();
					etile->SaveMod(root);
				}
				delete etile;
			}
		}
	}
}

ElevTile *ElevTile::Load(const std::string &root, int lvl, int ilat, int ilng)
{
	ElevTile *etile = new ElevTile(lvl, ilat, ilng);
	etile->Load(root);
	return etile;
}

void ElevTile::dataChanged(int exmin, int exmax, int eymin, int eymax)
{
	ExtractImage(exmin, exmax, eymin, eymax);
	m_modified = true;
}

void ElevTile::ExtractImage(int exmin, int exmax, int eymin, int eymax)
{
	img.width = (m_edata.width - 2) * 2 - 2;
	img.height = (m_edata.height - 2) * 2 - 2;
	img.data.resize(img.width * img.height);

	double dmin = m_edata.dmin;
	double dmax = m_edata.dmax;
	double dscale = (dmax > dmin ? 256.0 / (dmax - dmin) : 1.0);

	const Cmap &cm = cmap(CMAP_GREY);

	int imin = (exmin < 0 ? 0 : max(0, (exmin - 1) * 2 - 1));
	int imax = (exmax < 0 ? img.width : min((int)img.width, exmax * 2));
	int jmin = (eymax < 0 ? 0 : max(0, (int)img.height - (eymax - 1) * 2));
	int jmax = (eymin < 0 ? img.height : min((int)img.height, (int)img.height - (eymin - 1) * 2 + 1));

	for (int j = jmin; j < jmax; j++)
		for (int i = imin; i < imax; i++) {
			int ex = (i + 1) / 2 + 1;
			int ey = (img.height - j) / 2 + 1;
			double d = m_edata.data[ex + ey * m_edata.width];
			int v = min((int)((d - dmin) * dscale), 255);
			if (v < 0 || v > 255)
				std::cerr << "Problem" << std::endl;
			img.data[i + j * img.width] = (0xff000000 | cm[v]);
		}
}
