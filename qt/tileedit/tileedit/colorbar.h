#ifndef COLORBAR_H
#define COLORBAR_H

#include "QWidget"
#include "cmap.h"

class Colorbar : public QWidget
{
	Q_OBJECT

public:
	explicit Colorbar(QWidget *parent = 0);
	void setCmap(const Cmap *cmap);

protected:
	void paintEvent(QPaintEvent *event);

private:
	double m_vmin, m_vmax;
	const Cmap *m_cmap;
};

#endif // !COLORBAR_H