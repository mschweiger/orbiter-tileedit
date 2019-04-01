#ifndef DLGELEVCONFIG_H
#define DLGELEVCONFIG_H

#include <QDialog>

namespace Ui {
	class DlgElevConfig;
}

class tileedit;

class DlgElevConfig : public QDialog
{
	Q_OBJECT

public:
	DlgElevConfig(tileedit *parent);

public slots:
	void done(int r);
	void onColourmapChanged(int idx);

private:
	Ui::DlgElevConfig *ui;
	tileedit *m_tileedit;
};

#endif // !DLGELEVCONFIG_H
