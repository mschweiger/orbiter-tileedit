#ifndef DLGELEVIMPORT_H
#define DLGELEVIMPORT_H

#include <QDialog>

namespace Ui {
	class DlgElevImport;
}

class tileedit;
class ElevTileBlock;

struct ImageMetaInfo
{
	int lvl;
	int ilat0, ilat1, ilng0, ilng1;
	double dmin, dmax;
	double scale, offset;
	double latmin, latmax, lngmin, lngmax;
	int type;
	int colormap;
	std::vector<std::pair<int, int> > missing;
};

class DlgElevImport : public QDialog
{
	Q_OBJECT

public:
	DlgElevImport(tileedit *parent);

public slots:
	void onOpenFileDialog();
	void onOpenMetaFileDialog();
	void onMetaFileChanged(const QString&);

protected:
	bool scanMetaFile(const char *fname, ImageMetaInfo &meta);

private:
	Ui::DlgElevImport *ui;
	tileedit *m_tileedit;
	bool m_pathEdited, m_metaEdited;
	ImageMetaInfo m_metaInfo;
};

#endif // !DLGELEVIMPORT_H
