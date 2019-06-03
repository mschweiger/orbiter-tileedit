#ifndef DLGELEVEXPORT_H
#define DLGELEVEXPORT_H

#include <QDialog>

namespace Ui {
	class DlgElevExport;
}

class tileedit;
class ElevTileBlock;

class DlgElevExport : public QDialog
{
	Q_OBJECT

public:
	DlgElevExport(tileedit *parent);

private:
	bool saveWithAncestorData() const;
	void RescanLimits();

public slots:
	void onOpenFileDialog();
	void onSelectCurrentTiles();
	void onSelectCustomTiles();
	void onResolution(int);
	void onIlat0(int);
	void onIlat1(int);
	void onIlng0(int);
	void onIlng1(int);
	void onScaleAuto();
	void onScaleManual();
	void onImageValMin(double);
	void onImageValMax(double);
	void onImageResolution(double);
	void accept();

private:
	Ui::DlgElevExport *ui;
	std::string m_fileName;
	tileedit *m_tileedit;
	int m_lvl;
	int m_ilat0, m_ilat1;
	int m_ilng0, m_ilng1;
	double m_elevMin, m_elevMax;
	ElevTileBlock *m_CurrentBlock;
};

#endif // !DLGELEVEXPORT_H
