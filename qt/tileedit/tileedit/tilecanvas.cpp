#include <iostream>
#include "tilecanvas.h"
#include "QPainter"
#include "QResizeEvent"

TileCanvas::TileCanvas(QWidget *parent)
	: QWidget(parent)
{
    m_tile = 0;
	m_glyphMode = GLYPHMODE_NAVIGATE;
    overlay = new TileCanvasOverlay(this);
    overlay->hide();
    setMouseTracking(true);
}

TileCanvas::~TileCanvas()
{
}

void TileCanvas::resizeEvent(QResizeEvent *event)
{
    overlay->resize(event->size());
}

void TileCanvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QBrush brush(QColor(0,0,0));
    painter.setBrush(brush);
    painter.drawRect(rect());

    if (m_tile) {
        const Image &img = m_tile->getImage();
        const BYTE *data = (const BYTE*)img.data.data();
        QImage qimg(data, img.width, img.height, QImage::Format_ARGB32);
        painter.drawImage(rect(), qimg);
    }
}

void TileCanvas::enterEvent(QEvent *event)
{
    if (m_tile)
        overlay->show();
	emit tileEntered(this);
}

void TileCanvas::leaveEvent(QEvent *event)
{
    overlay->hide();
}

void TileCanvas::mouseMoveEvent(QMouseEvent *event)
{
    updateGlyph(event->x(), event->y());
	emit mouseMovedInCanvas(m_canvasIdx, event);
}

void TileCanvas::mousePressEvent(QMouseEvent *event)
{
	emit mousePressedInCanvas(m_canvasIdx, event);
}

void TileCanvas::mouseReleaseEvent(QMouseEvent *event)
{
	int lvl = m_lvl, ilat = m_ilat, ilng = m_ilng;

	TileCanvasOverlay::Glyph glyph = overlay->glyph();
	switch (glyph) {
	case TileCanvasOverlay::GLYPH_RECTFULL:
		lvl = m_lvl + 1;
		ilat = 0;
		ilng = 0;
		break;
	case TileCanvasOverlay::GLYPH_RECTLEFT:
		lvl = 4;
		ilat = 0;
		ilng = 0;
		break;
	case TileCanvasOverlay::GLYPH_RECTRIGHT:
		lvl = 4;
		ilat = 0;
		ilng = 1;
		break;
	case TileCanvasOverlay::GLYPH_RECTNW:
		lvl = m_lvl + 1;
		ilat = m_ilat * 2;
		ilng = m_ilng * 2;
		break;
	case TileCanvasOverlay::GLYPH_RECTNE:
		lvl = m_lvl + 1;
		ilat = m_ilat * 2;
		ilng = m_ilng * 2 + 1;
		break;
	case TileCanvasOverlay::GLYPH_RECTSW:
		lvl = m_lvl + 1;
		ilat = m_ilat * 2 + 1;
		ilng = m_ilng * 2;
		break;
	case TileCanvasOverlay::GLYPH_RECTSE:
		lvl = m_lvl + 1;
		ilat = m_ilat * 2 + 1;
		ilng = m_ilng * 2 + 1;
		break;
	case TileCanvasOverlay::GLYPH_ARROWLEFT:
		lvl = m_lvl;
		ilat = m_ilat;
		ilng = (m_ilng > 0 ? m_ilng - 1 : 0);
		break;
	case TileCanvasOverlay::GLYPH_ARROWRIGHT:
		lvl = m_lvl;
		ilat = m_ilat;
		ilng = (m_ilng < m_nlng - 1 ? m_ilng + 1 : m_nlng - 1);
		break;
	case TileCanvasOverlay::GLYPH_ARROWTOP:
		lvl = m_lvl;
		ilat = (m_ilat > 0 ? m_ilat - 1 : 0);
		ilng = m_ilng;
		break;
	case TileCanvasOverlay::GLYPH_ARROWBOTTOM:
		lvl = m_lvl;
		ilat = (m_ilat < m_nlat - 1 ? m_ilat + 1 : m_nlat - 1);
		ilng = m_ilng;
		break;
	case TileCanvasOverlay::GLYPH_CROSSCENTER:
		lvl = (m_lvl > 1 ? m_lvl - 1 : 1);
		ilat = m_ilat / 2;
		ilng = m_ilng / 2;
		break;
	}

	if (lvl != m_lvl || ilat != m_ilat || ilng != m_ilng) {
		emit tileChanged(lvl, ilat, ilng);
	}
	emit mouseReleasedInCanvas(m_canvasIdx, event);
}

void TileCanvas::updateGlyph(int x, int y)
{
    int w = rect().width();
    int h = rect().height();
    int dw = w/16;
    int dh = h/16;

	if (m_glyphMode == GLYPHMODE_NAVIGATE) {

		// check for zoom-out indicator
		if (m_lvl > 1) {
			if (x >= w / 2 - dw && x < w / 2 + dw && y >= h / 2 - dh && y < h / 2 + dh) {
				overlay->setGlyph(TileCanvasOverlay::GLYPH_CROSSCENTER);
				return;
			}
		}

		// check for pan left/right indicator
		if (m_ilng > 0 && x < dw && y >= h / 2 - dh && y < h / 2 + dh) {
			overlay->setGlyph(TileCanvasOverlay::GLYPH_ARROWLEFT);
			return;
		}
		else if (m_ilng < m_nlng - 1 && x >= w - dw && y >= h / 2 - dh && y < h / 2 + dh) {
			overlay->setGlyph(TileCanvasOverlay::GLYPH_ARROWRIGHT);
			return;
		}

		// check for pan up/down indicator
		if (m_ilat > 0 && y < dh && x >= w / 2 - dw && x < w / 2 + dw) {
			overlay->setGlyph(TileCanvasOverlay::GLYPH_ARROWTOP);
			return;
		}
		else if (m_ilat < m_nlat - 1 && y >= h - dh && x >= w / 2 - dw && x < w / 2 + dw) {
			overlay->setGlyph(TileCanvasOverlay::GLYPH_ARROWBOTTOM);
			return;
		}

		if (m_lvl < 3) {
			overlay->setGlyph(TileCanvasOverlay::GLYPH_RECTFULL);
		}
		else if (m_lvl == 3) {
			overlay->setGlyph(x < w / 2 ? TileCanvasOverlay::GLYPH_RECTLEFT : TileCanvasOverlay::GLYPH_RECTRIGHT);
		}
		else if (m_lvl < 19) {
			if (x < w / 2) {
				overlay->setGlyph(y < h / 2 ? TileCanvasOverlay::GLYPH_RECTNW : TileCanvasOverlay::GLYPH_RECTSW);
			}
			else {
				overlay->setGlyph(y < h / 2 ? TileCanvasOverlay::GLYPH_RECTNE : TileCanvasOverlay::GLYPH_RECTSE);
			}
		}
		else
			overlay->setGlyph(TileCanvasOverlay::GLYPH_NONE);
	}
	else {
		overlay->setGlyph(TileCanvasOverlay::GLYPH_NONE);
		overlay->setCrosshair(x, y);
	}
}

void TileCanvas::setImage(const Tile *tile)
{
    m_tile = tile;
    if (tile) {
        m_lvl = tile->Level();
        m_ilat = tile->iLat();
        m_ilng = tile->iLng();
        m_nlat = (m_lvl <= 4 ? 1 : 1 << (m_lvl-4));
        m_nlng = (m_lvl <= 3 ? 1 : 1 << (m_lvl-3));
    }
    //QPoint pos = mapFromGlobal(cursor().pos());
    //updateGlyph(pos.x(), pos.y());
    update();
}

void TileCanvas::setGlyphMode(GlyphMode mode)
{
	m_glyphMode = mode;
}



TileCanvasOverlay::TileCanvasOverlay(QWidget *parent): QWidget(parent)
{
    m_glyph = GLYPH_NONE;
    m_penGlyph.setColor(QColor(255,0,0));
    m_penGlyph.setWidth(3);
    m_penGlyph.setStyle(Qt::SolidLine);

    setMouseTracking(true);
}

void TileCanvasOverlay::setGlyph(Glyph glyph)
{
    if (glyph != m_glyph) {
        m_glyph = glyph;
        update();
    }
}

void TileCanvasOverlay::setCrosshair(int x, int y)
{

}

void TileCanvasOverlay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0,0,0,0));

    if(m_glyph != GLYPH_NONE) {
        painter.setPen(m_penGlyph);
        int w = rect().width();
        int h = rect().height();
        int dw = w/16;
        int dh = h/16;
        switch(m_glyph) {
        case GLYPH_RECTFULL:
            painter.drawRect(1, 1, w-3, h-3);
            break;
        case GLYPH_RECTLEFT:
            painter.drawRect(1, 1, w/2-3, h-3);
            break;
        case GLYPH_RECTRIGHT:
            painter.drawRect(w/2+2, 1, w/2-3, h-3);
            break;
        case GLYPH_RECTNW:
            painter.drawRect(1, 1, w/2-3, h/2-3);
            break;
        case GLYPH_RECTNE:
            painter.drawRect(w/2+2, 1, w/2-3, h/2-3);
            break;
        case GLYPH_RECTSW:
            painter.drawRect(1, h/2+2, w/2-3, h/2-3);
            break;
        case GLYPH_RECTSE:
            painter.drawRect(w/2+2, h/2+2, w/2-3, h/2-3);
            break;
        case GLYPH_ARROWTOP:
            painter.drawLine(w/2-dw, dh, w/2, 0);
            painter.drawLine(w/2+dw, dh, w/2, 0);
            break;
        case GLYPH_ARROWBOTTOM:
            painter.drawLine(w/2-dw, h-dh, w/2, h);
            painter.drawLine(w/2+dw, h-dh, w/2, h);
            break;
        case GLYPH_ARROWLEFT:
            painter.drawLine(dw, h/2-dh, 0, h/2);
            painter.drawLine(dw, h/2+dh, 0, h/2);
            break;
        case GLYPH_ARROWRIGHT:
            painter.drawLine(w-dw, h/2-dh, w, h/2);
            painter.drawLine(w-dw, h/2+dh, w, h/2);
            break;
        case GLYPH_CROSSCENTER:
            painter.drawLine(w/2-dw, h/2-dh, w/2+dw, h/2+dh);
            painter.drawLine(w/2-dw, h/2+dh, w/2+dw, h/2-dh);
            break;
        }
    }
}

void TileCanvasOverlay::mouseMoveEvent(QMouseEvent *event)
{
    ((TileCanvas*)parentWidget())->mouseMoveEvent(event);
}

void TileCanvasOverlay::mousePressEvent(QMouseEvent *event)
{
	((TileCanvas*)parentWidget())->mousePressEvent(event);
}

void TileCanvasOverlay::mouseReleaseEvent(QMouseEvent *event)
{
    ((TileCanvas*)parentWidget())->mouseReleaseEvent(event);
}
