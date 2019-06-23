#include "colorbar.h"
#include "cmap.h"
#include "QPainter"
#include "QResizeEvent"

Colorbar::Colorbar(QWidget *parent)
	: QWidget(parent)
{
	m_elevDisplayParam = 0;
	m_overlay = new ColorbarOverlay(this);
}

Colorbar::~Colorbar()
{
	delete m_overlay;
}

void Colorbar::setElevDisplayParam(const ElevDisplayParam &elevDisplayParam)
{
	m_elevDisplayParam = &elevDisplayParam;
	m_overlay->setRange(elevDisplayParam.rangeMin, elevDisplayParam.rangeMax);
}

void Colorbar::displayParamChanged()
{
	m_overlay->setRange(m_elevDisplayParam->rangeMin, m_elevDisplayParam->rangeMax);
	update();
}

void Colorbar::setValue(double val)
{
	m_overlay->setValue(val);
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

void Colorbar::resizeEvent(QResizeEvent *event)
{
	m_overlay->resize(event->size());
}



ColorbarOverlay::ColorbarOverlay(QWidget *parent)
	: QWidget(parent)
{
	m_vmin = 0.0;
	m_vmax = 1.0;
	m_val = 0.0;

	m_penIndicator0.setColor(QColor(255, 0, 0));
	m_penIndicator0.setWidth(1);
	m_penIndicator0.setStyle(Qt::SolidLine);
	m_penIndicator1.setColor(QColor(255, 255, 255));
	m_penIndicator1.setWidth(1);
	m_penIndicator1.setStyle(Qt::SolidLine);
}

void ColorbarOverlay::setRange(double vmin, double vmax)
{
	m_vmin = vmin;
	m_vmax = vmax;
	update();
}

void ColorbarOverlay::setValue(double val)
{
	m_val = val;
	update();
}

void ColorbarOverlay::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(0, 0, 0, 0));
	int w = rect().width();
	int h = rect().height();

	if (m_val != DBL_MAX) {
		int x = max(1, min(w - 2, (m_val - m_vmin) / (m_vmax - m_vmin) * w));
		painter.setPen(m_penIndicator0);
		painter.drawLine(x, 0, x, h);
		painter.setPen(m_penIndicator1);
		painter.drawLine(x - 1, 0, x - 1, h);
		painter.drawLine(x + 1, 0, x + 1, h);
	}
}