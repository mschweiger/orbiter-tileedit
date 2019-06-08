#include <iostream>
#include "tilecanvas.h"
#include "tileedit.h"
#include "QPainter"
#include "QResizeEvent"

TileCanvas::TileCanvas(QWidget *parent)
	: QWidget(parent)
{
	m_tileedit = 0;

    m_tileBlock = 0;
	m_glyphMode = GLYPHMODE_NAVIGATE;
    overlay = new TileCanvasOverlay(this);
    overlay->hide();
    setMouseTracking(true);
}

TileCanvas::~TileCanvas()
{
	delete overlay;
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

    if (m_tileBlock) {
        const Image &img = m_tileBlock->getImage();
        const BYTE *data = (const BYTE*)img.data.data();
        QImage qimg(data, img.width, img.height, QImage::Format_ARGB32);
        painter.drawImage(rect(), qimg);
    }
}

void TileCanvas::enterEvent(QEvent *event)
{
	emit tileEntered(this);
}

void TileCanvas::leaveEvent(QEvent *event)
{
	emit tileLeft(this);
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
	if (m_tileBlock) {
		int lvl = m_tileBlock->Level();
		int ilat0 = m_tileBlock->iLat0();
		int ilat1 = m_tileBlock->iLat1();
		int ilng0 = m_tileBlock->iLng0();
		int ilng1 = m_tileBlock->iLng1();
		int nlat = m_tileBlock->nLat();
		int nlng = m_tileBlock->nLng();

		TileCanvasOverlay::Glyph glyph = overlay->glyph();
		switch (glyph) {
		case TileCanvasOverlay::GLYPH_RECTFULL:
			lvl++;
			ilat0 = 0;
			ilng0 = 0;
			break;
		case TileCanvasOverlay::GLYPH_RECTLEFT:
			lvl++;
			ilat0 = 0;
			ilng0 = 0;
			break;
		case TileCanvasOverlay::GLYPH_RECTRIGHT:
			lvl++;
			ilat0 = 0;
			ilng0 = m_tileedit->m_blocksize;
			break;
		case TileCanvasOverlay::GLYPH_RECTNW:
			lvl++;
			ilat0 = ilat0 * 2;
			ilng0 = ilng0 * 2;
			break;
		case TileCanvasOverlay::GLYPH_RECTNE:
			lvl++;
			ilat0 = ilat0 * 2;
			ilng0 = ilng0 * 2 + m_tileedit->m_blocksize;
			break;
		case TileCanvasOverlay::GLYPH_RECTSW:
			lvl++;
			ilat0 = ilat0 * 2 + m_tileedit->m_blocksize;
			ilng0 = ilng0 * 2;
			break;
		case TileCanvasOverlay::GLYPH_RECTSE:
			lvl++;
			ilat0 = ilat0 * 2 + m_tileedit->m_blocksize;
			ilng0 = ilng0 * 2 + m_tileedit->m_blocksize;
			break;
		case TileCanvasOverlay::GLYPH_ARROWLEFT:
			ilng0 = (ilng0 > 0 ? ilng0 - 1 : 0);
			break;
		case TileCanvasOverlay::GLYPH_ARROWRIGHT:
			ilng0 = (ilng1 < nlng ? ilng0 + 1 : nlng - 1);
			break;
		case TileCanvasOverlay::GLYPH_ARROWTOP:
			ilat0 = (ilat0 > 0 ? ilat0 - 1 : 0);
			break;
		case TileCanvasOverlay::GLYPH_ARROWBOTTOM:
			ilat0 = (ilat1 < nlat ? ilat0 + 1 : nlat - 1);
			break;
		case TileCanvasOverlay::GLYPH_CROSSCENTER:
			lvl = (lvl > 1 ? lvl - 1 : 1);
			ilat0 /= 2;
			ilng0 /= 2;
			ilng1 = ilng0 + m_tileedit->m_blocksize;
			if (ilng1 > nLng(lvl) && ilng0)
				ilng0--;
			break;
		}

		if (lvl != m_tileBlock->Level() ||
			ilat0 != m_tileBlock->iLat0() ||
			ilng0 != m_tileBlock->iLng0()) {
			emit tileChanged(lvl, ilat0, ilng0);
		}
	}
	emit mouseReleasedInCanvas(m_canvasIdx, event);
}

void TileCanvas::updateGlyph(int x, int y)
{
    int w = rect().width();
    int h = rect().height();
    int dw = w/16;
    int dh = h/16;

	if (m_glyphMode == GLYPHMODE_NAVIGATE && m_tileBlock) {

		int nlat = m_tileBlock->nLat();
		int nlng = m_tileBlock->nLng();

		// check for zoom-out indicator
		if (m_tileBlock->Level() > 1) {
			if (x >= w / 2 - dw && x < w / 2 + dw && y >= h / 2 - dh && y < h / 2 + dh) {
				overlay->setGlyph(TileCanvasOverlay::GLYPH_CROSSCENTER);
				return;
			}
		}

		// check for pan left/right indicator
		if (m_tileBlock->iLng0() > 0 && x < dw && y >= h / 2 - dh && y < h / 2 + dh) {
			overlay->setGlyph(TileCanvasOverlay::GLYPH_ARROWLEFT);
			return;
		}
		else if (m_tileBlock->iLng1() < nlng && x >= w - dw && y >= h / 2 - dh && y < h / 2 + dh) {
			overlay->setGlyph(TileCanvasOverlay::GLYPH_ARROWRIGHT);
			return;
		}

		// check for pan up/down indicator
		if (m_tileBlock->iLat0() > 0 && y < dh && x >= w / 2 - dw && x < w / 2 + dw) {
			overlay->setGlyph(TileCanvasOverlay::GLYPH_ARROWTOP);
			return;
		}
		else if (m_tileBlock->iLat1() < nlat && y >= h - dh && x >= w / 2 - dw && x < w / 2 + dw) {
			overlay->setGlyph(TileCanvasOverlay::GLYPH_ARROWBOTTOM);
			return;
		}

		if (nLng(m_tileBlock->Level() + 1) <= m_tileedit->m_blocksize) {
			overlay->setGlyph(TileCanvasOverlay::GLYPH_RECTFULL);
		}
		else if (nLat(m_tileBlock->Level() + 1) <= m_tileedit->m_blocksize) {
		//else if (m_tileBlock->Level() == 3) {
			overlay->setGlyph(x < w / 2 ? TileCanvasOverlay::GLYPH_RECTLEFT : TileCanvasOverlay::GLYPH_RECTRIGHT);
		}
		else if (m_tileBlock->Level() < 19) {
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
	}
}

void TileCanvas::setTileBlock(const TileBlock *tileBlock)
{
	m_tileBlock = tileBlock;
	if (m_glyphMode == GLYPHMODE_NAVIGATE) {
		QPoint pos = mapFromGlobal(cursor().pos());
		updateGlyph(pos.x(), pos.y());
	}
	overlay->setTileBlock(tileBlock);
	update();
}

void TileCanvas::setGlyphMode(GlyphMode mode)
{
	m_glyphMode = mode;
	overlay->setCursor(mode == GLYPHMODE_NAVIGATE ? Qt::ArrowCursor : Qt::BlankCursor);
}

void TileCanvas::setCrosshair(double x, double y, double rad)
{
	if (m_tileBlock)
		overlay->setCrosshair(x, y, rad);
}

void TileCanvas::showOverlay(bool show)
{
	if (show) {
		if (m_tileBlock)
			overlay->show();
	}
	else {
		overlay->hide();
	}
}


QFont TileCanvasOverlay::s_font = QFont("Courier", 10);

TileCanvasOverlay::TileCanvasOverlay(QWidget *parent)
	: QWidget(parent)
{
    m_glyph = GLYPH_NONE;
    m_penGlyph.setColor(QColor(255,0,0));
    m_penGlyph.setWidth(3);
    m_penGlyph.setStyle(Qt::SolidLine);

	m_penCrosshair.setColor(QColor(255, 0, 0, 128));
	m_penCrosshair.setWidth(1);
	m_penCrosshair.setStyle(Qt::SolidLine);

	m_tileBlock = 0;

    setMouseTracking(true);
}

void TileCanvasOverlay::setGlyph(Glyph glyph)
{
    if (glyph != m_glyph) {
        m_glyph = glyph;
        update();
    }
}

void TileCanvasOverlay::setCrosshair(double x, double y, double rad)
{
	if (m_glyph != GLYPH_CROSSHAIR || m_crosshairX != x || m_crosshairY != y) {
		m_glyph = GLYPH_CROSSHAIR;
		m_crosshairX = x;
		m_crosshairY = y;
		m_crosshairR = rad;
		update();
	}
}

void TileCanvasOverlay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
	painter.setClipping(true);
    painter.fillRect(rect(), QColor(0,0,0,0));

    if(m_glyph != GLYPH_NONE) {
        painter.setPen(m_glyph == GLYPH_CROSSHAIR ? m_penCrosshair : m_penGlyph);
        int w = rect().width();
        int h = rect().height();
        int dw = w/32;
        int dh = h/32;
		char cbuf[256];
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
		case GLYPH_CROSSHAIR:
			{
			int x = (int)(m_crosshairX * w);
			int y = (int)(m_crosshairY * h);
			int dx = (int)(m_crosshairR * w + 0.5) + 1;
			int dy = (int)(m_crosshairR * h + 0.5) + 1;
			painter.drawEllipse(x - dx, y - dy, dx * 2, dy * 2);
			painter.drawLine(x - dx, y, x - dx - dw, y);
			painter.drawLine(x + dx, y, x + dx + dw, y);
			painter.drawLine(x, y - dy, x, y - dy - dh);
			painter.drawLine(x, y + dy, x, y + dy + dh);

			//painter.drawLine(x - dw, y, x - dw / 2, y);
			//painter.drawLine(x + dw, y, x + dw / 2, y);
			//painter.drawLine(x, y - dh, x, y - dh / 2);
			//painter.drawLine(x, y + dh, x, y + dh / 2);
			}
			break;
        }
		if (m_tileBlock && m_glyph != GLYPH_CROSSHAIR) {
			if (m_tileBlock->nLngBlock() == 2 || m_tileBlock->nLatBlock() == 2) {
				painter.setPen(m_penCrosshair);
				if (m_tileBlock->nLngBlock() == 2)
					painter.drawLine(w / 2, 0, w / 2, h);
				if (m_tileBlock->nLatBlock() == 2)
					painter.drawLine(0, h / 2, w, h / 2);
			}
			painter.setFont(s_font);
			for (int ilat = m_tileBlock->iLat0(); ilat < m_tileBlock->iLat1(); ilat++) {
				int y = ((ilat - m_tileBlock->iLat0()) * h) / (m_tileBlock->nLatBlock()) + 18;
				for (int ilng = m_tileBlock->iLng0(); ilng < m_tileBlock->iLng1(); ilng++) {
					int x = ((ilng - m_tileBlock->iLng0()) * w) / (m_tileBlock->nLngBlock()) + 6;
					const Tile *tile = m_tileBlock->getTile(ilat, ilng);
					if (tile) {
						sprintf(cbuf, " %02d/%06d/%06d", tile->Level(), tile->iLat(), tile->iLng());
						painter.drawText(x, y, QString(cbuf));
						if (tile->subLevel() < tile->Level()) {
							sprintf(cbuf, "[%02d/%06d/%06d]", tile->subLevel(), tile->subiLat(), tile->subiLng());
							painter.drawText(x, y+16, QString(cbuf));
						}
					}
				}
			}
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
