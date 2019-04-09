#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <random>

#include <QMainWindow>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>

#include "elevtile.h"

namespace Ui {
class tileedit;
}

class SurfTile;
class MaskTile;
class NightlightTile;
class ElevTile;
class TileCanvas;
class DlgElevConfig;

class tileedit : public QMainWindow
{
	Q_OBJECT

	friend class DlgElevConfig;

public:
    explicit tileedit(QWidget *parent = 0);
    ~tileedit();

	void elevDisplayParamChanged();

protected:
    void createActions();
    void createMenus();
    void resizeEvent(QResizeEvent *event);
    void ensureSquareCanvas(int winw, int winh);
    void loadTile(int lvl, int ilat, int ilng);
    void refreshPanel(int panelIdx);
	void setTile(int lvl, int ilat, int ilng);

private:
	void setToolOptions();
	QString ModeString() const;
	std::pair<int, int> ElevNodeFromPixCoord(int canvasIdx, int x, int y);
	void editElevation(int canvasIdx, int x, int y);

private slots:
    void openDir();
	void on_actionExit_triggered();
	void onElevConfig();
	void onElevConfigDestroyed(int r);
    void onResolutionChanged(int val);
    void onLatidxChanged(int val);
    void onLngidxChanged(int val);
	void onActionButtonClicked(int id);
	void onEditButtonClicked(int id);
    void onLayerType0(int);
    void onLayerType1(int);
    void onLayerType2(int);
    void OnTileChangedFromPanel(int lvl, int ilat, int ilng);
	void OnTileEntered(TileCanvas *canvas);
	void OnTileLeft(TileCanvas *canvas);
	void OnMouseMovedInCanvas(int canvasIdx, QMouseEvent *event);
	void OnMousePressedInCanvas(int canvasIdx, QMouseEvent *event);
	void OnMouseReleasedInCanvas(int canvasIdx, QMouseEvent *event);

private:
    Ui::tileedit *ui;
    QMenu *fileMenu;
    QAction *openAct;
	QAction *actionExit;
	QAction *actionElevConfig;

    QLabel *status;

    struct TilePanel {
        TileCanvas *canvas;
		QWidget *colourscale;
		QLabel *rangeMin;
		QLabel *rangeMax;
        QComboBox *layerType;
		QLabel *fileId;
    } m_panel[3];

	enum ActionMode {
		ACTION_NAVIGATE,
		ACTION_ELEVEDIT
	} m_actionMode;

	enum ElevEditMode {
		ELEVEDIT_PAINT,
		ELEVEDIT_RANDOM,
		ELEVEDIT_ERASE
	} m_elevEditMode;

    std::string rootDir;
    int m_lvl;
    int m_ilat;
    int m_ilng;

	ElevDisplayParam m_elevDisplayParam;

	bool m_mouseDown;

    SurfTile *m_stile;
	MaskTile *m_mtile;
	NightlightTile *m_ltile;
	ElevTile *m_etile;
	ElevTile *m_etileRef;

	DlgElevConfig *m_dlgElevConfig;

	std::normal_distribution<double> *m_rndn;
};

#endif // MAINWINDOW_H
