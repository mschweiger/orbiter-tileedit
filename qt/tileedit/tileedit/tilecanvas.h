#ifndef TILECANVAS_H
#define TILECANVAS_H

#include <windows.h>
#include "QWidget"
#include "QBoxLayout"
#include "QPen"
#include "tile.h"

class TileCanvasOverlay;

class TileCanvas: public QWidget
{
    Q_OBJECT

    friend class TileCanvasOverlay;

public:
	enum GlyphMode {
		GLYPHMODE_NAVIGATE,
		GLYPHMODE_CROSSHAIR
	};

	explicit TileCanvas(QWidget *parent = 0);
    ~TileCanvas();

	const Tile *tile() const{ return m_tile; }
	int idx() const { return m_canvasIdx; }
	void setIdx(int idx) { m_canvasIdx = idx; }
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void setImage(const Tile *tile);
	void setGlyphMode(GlyphMode mode);
	void setCrosshair(double x, double y);
	void showOverlay(bool show);

protected:
    void updateGlyph(int mx, int my);
    TileCanvasOverlay *overlay;

private:
    const Tile *m_tile;
	int m_canvasIdx;
    int m_lvl;
    int m_ilat;
    int m_ilng;
    int m_nlat;
    int m_nlng;
	GlyphMode m_glyphMode;

signals:
    void tileChanged(int lvl, int ilat, int ilng);
	void tileEntered(TileCanvas *canvas);
	void tileLeft(TileCanvas *canvas);
	void mouseMovedInCanvas(int canvasIdx, QMouseEvent *event);
	void mousePressedInCanvas(int canvasIdx, QMouseEvent *event);
	void mouseReleasedInCanvas(int canvasIdx, QMouseEvent *event);
};

class TileCanvasOverlay: public QWidget
{
    Q_OBJECT

public:
    enum Glyph {
        GLYPH_NONE,
        GLYPH_RECTFULL,
        GLYPH_RECTLEFT,
        GLYPH_RECTRIGHT,
        GLYPH_RECTNW,
        GLYPH_RECTNE,
        GLYPH_RECTSW,
        GLYPH_RECTSE,
        GLYPH_ARROWTOP,
        GLYPH_ARROWBOTTOM,
        GLYPH_ARROWLEFT,
        GLYPH_ARROWRIGHT,
        GLYPH_CROSSCENTER,
		GLYPH_CROSSHAIR
    };

    explicit TileCanvasOverlay(QWidget *parent = 0);
    Glyph glyph() const { return m_glyph; }
    void setGlyph(Glyph glyph);
	void setCrosshair(double x, double y);
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Glyph m_glyph;
    QPen m_penGlyph;
	QPen m_penCrosshair;
	double m_crosshairX, m_crosshairY;
};

#endif // TILECANVAS_H

