#include "dlgelevconfig.h"
#include "ui_dlgElevConfig.h"
#include "tileedit.h"

DlgElevConfig::DlgElevConfig(tileedit *parent)
	: QDialog(parent)
	, m_tileedit(parent)
	, ui(new Ui::DlgElevConfig)
{
	ui->setupUi(this);
	ui->comboColourmap->setCurrentIndex(m_tileedit->m_colourMapIdx);
	connect(ui->comboColourmap, SIGNAL(currentIndexChanged(int)), this, SLOT(onColourmapChanged(int)));
}

void DlgElevConfig::done(int r)
{
	emit finished(r);
}

void DlgElevConfig::onColourmapChanged(int idx)
{
	m_tileedit->setColourmap(idx);
}