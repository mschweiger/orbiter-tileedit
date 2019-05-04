#include "dlgconfig.h"
#include "ui_dlgConfig.h"
#include "tileedit.h"

DlgConfig::DlgConfig(tileedit *parent)
	: QDialog(parent)
	, m_tileedit(parent)
	, ui(new Ui::DlgConfig)
{
	ui->setupUi(this);

	DWORD flag = m_tileedit->m_openMode;
	ui->comboLoadSequence->setCurrentIndex(flag == 1 ? 2 : flag == 2 ? 1 : 0);
	ui->comboDisplayMode->setCurrentIndex(m_tileedit->m_blocksize == 1 ? 0 : 1);
}

void DlgConfig::accept()
{
	DWORD loadFlag = 0;
	int idx = ui->comboLoadSequence->currentIndex();
	if (idx != 1) loadFlag |= 0x1;
	if (idx != 2) loadFlag |= 0x2;

	m_tileedit->setLoadMode(loadFlag);

	idx = ui->comboDisplayMode->currentIndex();
	m_tileedit->setBlockSize(idx == 0 ? 1 : 2);

	QDialog::accept();
}