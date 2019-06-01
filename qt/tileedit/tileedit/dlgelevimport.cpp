#include "dlgelevimport.h"
#include "ui_dlgElevImport.h"
#include "tileedit.h"
#include "tileblock.h"

#include <QFileDialog>
#include <QMessageBox>

DlgElevImport::DlgElevImport(tileedit *parent)
	: QDialog(parent)
	, m_tileedit(parent)
	, ui(new Ui::DlgElevImport)
{
	ui->setupUi(this);

	connect(ui->pushOpenFileDialog, SIGNAL(clicked()), this, SLOT(onOpenFileDialog()));
	connect(ui->pushOpenMetaFileDialog, SIGNAL(clicked()), this, SLOT(onOpenMetaFileDialog()));
	connect(ui->editMetaPath, SIGNAL(textChanged(const QString&)), this, SLOT(onMetaFileChanged(const QString&)));

	m_pathEdited = m_metaEdited = false;
}

void DlgElevImport::onOpenFileDialog()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Import elevation tiles from image file"), ui->editPath->text(), tr("Portable network graphics (*.png)"));
	if (path.size()) {
		ui->editPath->setText(path);
		if (!m_metaEdited) {
			path.append(".hdr");
			ui->editMetaPath->setText(path);
		}
		m_pathEdited = true;
	}
}

void DlgElevImport::onOpenMetaFileDialog()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Import elevation tiles from image file"), ui->editMetaPath->text(), tr("Metadata file (*.hdr)"));
	if (path.size()) {
		ui->editMetaPath->setText(path);
		if (!m_pathEdited) {
			path.truncate(path.length()-4);
			ui->editPath->setText(path);
		}
		m_metaEdited = true;
	}
}

void DlgElevImport::onMetaFileChanged(const QString &name)
{
	bool found = scanMetaFile(name.toLatin1(), m_metaInfo);
	if (found) {
		ui->labelLvl->setText(QString::number(m_metaInfo.lvl));
		ui->spinIlat0->setValue(m_metaInfo.ilat0);
		ui->spinIlat1->setValue(m_metaInfo.ilat1 - 1);
		ui->spinIlng0->setValue(m_metaInfo.ilng0);
		ui->spinIlng1->setValue(m_metaInfo.ilng1 - 1);
	}
	else {
		ui->labelLvl->setText("-");
		ui->spinIlat0->setValue(0);
		ui->spinIlat1->setValue(0);
		ui->spinIlng0->setValue(0);
		ui->spinIlng1->setValue(0);
	}
}

bool DlgElevImport::scanMetaFile(const char *fname, ImageMetaInfo &meta)
{
	FILE *f = fopen(fname, "rt");
	if (!f) return false;

	int ilat, ilng, n;
	double smin, emin, smean, emean, smax, emax;
	char str[1024];
	fscanf(f, "vmin=%lf vmax=%lf scale=%lf offset=%lf type=%d padding=1x1 colormap=%d smin=%lf emin=%lf smean=%lf emean=%lf smax=%lf emax=%lf latmin=%lf latmax=%lf lngmin=%lf lngmax=%lf\n",
		&meta.dmin, &meta.dmax, &meta.scale, &meta.offset, &meta.type, &meta.colormap, &smin, &emin, &smean, &emean, &smax, &emax,
		&meta.latmin, &meta.latmax, &meta.lngmin, &meta.lngmax);
	fscanf(f, "lvl=%d ilat0=%d ilat1=%d ilng0=%d ilng1=%d\n",
		&meta.lvl, &meta.ilat0, &meta.ilat1, &meta.ilng0, &meta.ilng1);
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
	fclose(f);
	return true;
}