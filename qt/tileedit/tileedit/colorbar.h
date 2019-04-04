#ifndef COLORBAR_H
#define COLORBAR_H

#include "QWidget"
#include "QPen"
#include "cmap.h"
#include "elevtile.h"

class ColorbarOverlay;

class Colorbar : public QWidget
{
	Q_OBJECT

public:
	explicit Colorbar(QWidget *parent = 0);
	~Colorbar();
	void setElevDisplayParam(const ElevDisplayParam &elevDisplayParam);
	void displayParamChanged();
	void setValue(double val);

protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);

private:
	const ElevDisplayParam *m_elevDisplayParam;
	ColorbarOverlay *m_overlay;
};


class ColorbarOverlay : public QWidget
{
	Q_OBJECT

public:
	explicit ColorbarOverlay(QWidget *parent = 0);
	void setRange(double vmin, double vmax);
	void setValue(double val);

protected:
	void paintEvent(QPaintEvent *event);

private:
	double m_vmin, m_vmax;
	double m_val;
	QPen m_penIndicator0;
	QPen m_penIndicator1;
};

#endif // !COLORBAR_H