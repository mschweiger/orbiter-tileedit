#ifndef COLORBAR_H
#define COLORBAR_H

#include "QWidget"
#include "cmap.h"
#include "elevtile.h"

class Colorbar : public QWidget
{
	Q_OBJECT

public:
	explicit Colorbar(QWidget *parent = 0);
	void setElevDisplayParam(const ElevDisplayParam &elevDisplayParam);
	void displayParamChanged();

protected:
	void paintEvent(QPaintEvent *event);

private:
	double m_vmin, m_vmax;
	const ElevDisplayParam *m_elevDisplayParam;
};

#endif // !COLORBAR_H