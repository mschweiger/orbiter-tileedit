#include "dlgelevexport.h"
#include "ui_dlgElevExport.h"
#include "tileedit.h"
#include "tileblock.h"

#include <QFileDialog>

DlgElevExport::DlgElevExport(tileedit *parent)
	: QDialog(parent)
	, m_tileedit(parent)
	, ui(new Ui::DlgElevExport)
{
	ui->setupUi(this);

	connect(ui->pushOpenFileDialog, SIGNAL(clicked()), this, SLOT(onOpenFileDialog()));
	connect(ui->radioCurrentTiles, SIGNAL(clicked()), this, SLOT(onSelectCurrentTiles()));
	connect(ui->radioCustomTiles, SIGNAL(clicked()), this, SLOT(onSelectCustomTiles()));
	connect(ui->spinLvl, SIGNAL(valueChanged(int)), this, SLOT(onResolution(int)));
	connect(ui->spinIlat0, SIGNAL(valueChanged(int)), this, SLOT(onIlat0(int)));
	connect(ui->spinIlat1, SIGNAL(valueChanged(int)), this, SLOT(onIlat1(int)));
	connect(ui->spinIlng0, SIGNAL(valueChanged(int)), this, SLOT(onIlng0(int)));
	connect(ui->spinIlng1, SIGNAL(valueChanged(int)), this, SLOT(onIlng1(int)));

	m_CurrentBlock = m_tileedit->m_eTileBlock;
	if (m_CurrentBlock) {
		m_lvl = m_CurrentBlock->Level();
		m_ilat0 = m_CurrentBlock->iLat0();
		m_ilat1 = m_CurrentBlock->iLat1();
		m_ilng0 = m_CurrentBlock->iLng0();
		m_ilng1 = m_CurrentBlock->iLng1();
	}
	else {
		m_lvl = 1;
		m_ilat0 = m_ilat1 = m_ilng0 = m_ilng1 = 0;
	}
	ui->spinLvl->setValue(m_lvl);
	ui->spinIlat0->setValue(m_ilat0);
	ui->spinIlat1->setValue(m_ilat1 - 1);
	ui->spinIlng0->setValue(m_ilng0);
	ui->spinIlng1->setValue(m_ilng1 - 1);
	ui->spinIlat0->setMaximum(nLat(m_lvl) - 1);
	ui->spinIlat1->setMaximum(nLat(m_lvl) - 1);
	ui->spinIlng0->setMaximum(nLng(m_lvl) - 1);
	ui->spinIlng1->setMaximum(nLng(m_lvl) - 1);

	char path[1024], drive[16], dir[1024], name[1024], ext[1024];
	GetModuleFileNameA(NULL, path, 1024);
	_splitpath(path, drive, dir, name, ext);
	sprintf(path, "%s%selev_%02d_%06d_%06d.png", drive, dir, m_lvl, m_ilat0, m_ilng0);
	ui->editPath->setText(path);
}

void DlgElevExport::onOpenFileDialog()
{
	std::string path = QFileDialog::getSaveFileName(this, tr("Export elevation tile to PNG"), ui->editPath->text(), tr("Portable network graphics (*.png)")).toStdString();
	if (path.size()) {
		ui->editPath->setText(QString::fromStdString(path));
	}

}

void DlgElevExport::onSelectCurrentTiles()
{
	ui->widgetTileRange->setEnabled(false);
}

void DlgElevExport::onSelectCustomTiles()
{
	ui->widgetTileRange->setEnabled(true);
}

void DlgElevExport::onResolution(int value)
{
	m_lvl = value;
	ui->spinIlat0->setMaximum(nLat(m_lvl) - 1);
	ui->spinIlat1->setMaximum(nLat(m_lvl) - 1);
	ui->spinIlng0->setMaximum(nLng(m_lvl) - 1);
	ui->spinIlng1->setMaximum(nLng(m_lvl) - 1);
}

void DlgElevExport::onIlat0(int value)
{
	m_ilat0 = value;
}

void DlgElevExport::onIlat1(int value)
{
	m_ilat1 = value + 1;
}

void DlgElevExport::onIlng0(int value)
{
	m_ilng0 = value;
}

void DlgElevExport::onIlng1(int value)
{
	m_ilng1 = value + 1;
}

void DlgElevExport::accept()
{
	if (ui->radioCurrentTiles->isChecked()) {
		m_CurrentBlock->ExportPNG(ui->editPath->text().toStdString());
	}
	else {
		ElevTileBlock *eblock = ElevTileBlock::Load(m_lvl, m_ilat0, m_ilat1, m_ilng0, m_ilng1);
		eblock->ExportPNG(ui->editPath->text().toStdString());
		delete eblock;
	}
	QDialog::accept();
}