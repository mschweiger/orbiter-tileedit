#include "dlgsurfimport.h"
#include "ui_dlgSurfImport.h"
#include "tileedit.h"
#include "tileblock.h"

#include <QMessageBox>

DlgSurfImport::DlgSurfImport(tileedit *parent)
	: QDialog(parent)
	, ui(new Ui::DlgSurfImport)
{
	ui->setupUi(this);

	connect(ui->radioParamFromMeta, SIGNAL(clicked()), this, SLOT(onParamFromMeta()));
	connect(ui->radioParamFromUser, SIGNAL(clicked()), this, SLOT(onParamFromUser()));
	connect(ui->editMetaPath, SIGNAL(textChanged(const QString&)), this, SLOT(onMetaFileChanged(const QString&)));
	connect(ui->spinLvl, SIGNAL(valueChanged(int)), this, SLOT(onLvl(int)));

	m_haveMeta = false;
	memset(&m_metaInfo, 0, sizeof(SurfPatchMetaInfo));
}

void DlgSurfImport::onParamFromMeta()
{
	ui->widgetParamMeta->setEnabled(true);
	ui->widgetParamUser->setEnabled(false);
}

void DlgSurfImport::onParamFromUser()
{
	ui->widgetParamMeta->setEnabled(false);
	ui->widgetParamUser->setEnabled(true);
}

void DlgSurfImport::onMetaFileChanged(const QString &name)
{
	m_haveMeta = scanMetaFile(name.toLatin1(), m_metaInfo);
	if (m_haveMeta) {
		ui->spinLvl->setValue(m_metaInfo.lvl);
		ui->spinIlat0->setValue(m_metaInfo.ilat0);
		ui->spinIlat1->setValue(m_metaInfo.ilat1 - 1);
		ui->spinIlng0->setValue(m_metaInfo.ilng0);
		ui->spinIlng1->setValue(m_metaInfo.ilng1);
	}
	else {
		memset(&m_metaInfo, 0, sizeof(SurfPatchMetaInfo));
		ui->spinLvl->setValue(1);
		ui->spinIlat0->setValue(0);
		ui->spinIlat1->setValue(0);
		ui->spinIlng0->setValue(0);
		ui->spinIlng1->setValue(0);
	}
}

void DlgSurfImport::onLvl(int val)
{
	int nlat = nLat(val);
	int nlng = nLng(val);
	ui->spinIlat1->setMaximum(nlat - 1);
	ui->spinIlng1->setMaximum(nlng - 1);
}

void DlgSurfImport::accept()
{
	if (ui->radioParamFromUser->isChecked()) {
		m_metaInfo.lvl = ui->spinLvl->value();
		m_metaInfo.ilat0 = ui->spinIlat0->value();
		m_metaInfo.ilat1 = ui->spinIlat1->value() + 1;
		m_metaInfo.ilng0 = ui->spinIlng0->value();
		m_metaInfo.ilng1 = ui->spinIlng1->value() + 1;
	}
	else {
		if (!m_haveMeta) {
			QMessageBox mbox(QMessageBox::Warning, tr("tileedit: Warning"), tr("No valid metadata information is available"), QMessageBox::Close);
			mbox.exec();
			return;
		}
	}

	SurfTileBlock *sblock = SurfTileBlock::Load(m_metaInfo.lvl, m_metaInfo.ilat0, m_metaInfo.ilat1, m_metaInfo.ilng0, m_metaInfo.ilng1);
	if (!dxtread_png(ui->editPath->text().toLatin1(), m_metaInfo, sblock->getData())) {
		QMessageBox mbox(QMessageBox::Warning, tr("tileedit: Warning"), tr("Error reading PNG file"), QMessageBox::Close);
		mbox.exec();
		return;
	}

	for (int ilat = m_metaInfo.ilat0; ilat < m_metaInfo.ilat1; ilat++)
		for (int ilng = m_metaInfo.ilng0; ilng < m_metaInfo.ilng1; ilng++) {
			sblock->syncTile(ilat, ilng);
			SurfTile *stile = (SurfTile*)sblock->_getTile(ilat, ilng);
			if (stile->Level() == stile->subLevel())
				stile->Save();
			// ...
		}

	QDialog::accept();
}

bool DlgSurfImport::scanMetaFile(const char *fname, SurfPatchMetaInfo &meta)
{
	FILE *f = fopen(fname, "rt");
	if (!f) return false;

	int ilat, ilng, n;
	char str[1024];
	if (fscanf(f, "lvl=%d ilat0=%d ilat1=%d ilng0=%d ilng1=%d\n",
		&meta.lvl, &meta.ilat0, &meta.ilat1, &meta.ilng0, &meta.ilng1) != 5) {
		meta.lvl = meta.ilat0 = meta.ilat1 = meta.ilng0 = meta.ilng1 = 0;
	}
	else {
		fscanf(f, "%s", str);
		if (!strncmp(str, "missing", 7)) {
			while (true) {
				n = fscanf(f, "%d/%d", &ilat, &ilng);
				if (n == 2) {
					meta.missing.push_back(std::make_pair(ilat, ilng));
				}
				else
					break;
			}
		}
	}
	fclose(f);
	return true;
}