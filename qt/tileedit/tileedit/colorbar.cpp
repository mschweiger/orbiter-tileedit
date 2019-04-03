#include "colorbar.h"
#include "cmap.h"
#include "QPainter"

Colorbar::Colorbar(QWidget *parent)
	: QWidget(parent)
{
	m_vmin = 0.0;
	m_vmax = 1.0;
	m_elevDisplayParam = 0;
}

void Colorbar::setElevDisplayParam(const ElevDisplayParam &elevDisplayParam)
{
	m_elevDisplayParam = &elevDisplayParam;
}

void Colorbar::displayParamChanged()
{
	update();
}

void Colorbar::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	QBrush brush(QColor(0, 0, 0));
	painter.setBrush(brush);
	painter.drawRect(rect());

	if (m_elevDisplayParam) {
		const Cmap &cm = cmap(m_elevDisplayParam->cmName);
		DWORD data[256];
		memcpy(data, cm, 256 * sizeof(DWORD));
		for (int i = 0; i < 256; i++)
			data[i] |= 0xff000000;
		QImage qimg((BYTE*)data, 256, 1, QImage::Format_ARGB32);
		painter.drawImage(rect(), qimg);
	}
	else {
		QBrush brush(QColor(255, 255, 255));
		painter.setBrush(brush);
		painter.drawRect(rect());
	}
}