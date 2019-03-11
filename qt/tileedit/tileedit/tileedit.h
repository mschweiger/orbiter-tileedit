#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <random>

#include <QMainWindow>
#include <QLabel>
#include <QComboBox>

namespace Ui {
class tileedit;
}

class SurfTile;
class MaskTile;
class NightlightTile;
class ElevTile;
class TileCanvas;

class tileedit : public QMainWindow
{
    Q_OBJECT

public:
    explicit tileedit(QWidget *parent = 0);
    ~tileedit();

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
	void OnMouseMovedInCanvas(int canvasIdx, QMouseEvent *event);
	void OnMousePressedInCanvas(int canvasIdx, QMouseEvent *event);
	void OnMouseReleasedInCanvas(int canvasIdx, QMouseEvent *event);

private:
    Ui::tileedit *ui;
    QMenu *fileMenu;
    QAction *openAct;
    QLabel *status;

    struct TilePanel {
        TileCanvas *canvas;
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

	bool m_mouseDown;

    SurfTile *m_stile;
	MaskTile *m_mtile;
	NightlightTile *m_ltile;
	ElevTile *m_etile;
	ElevTile *m_etileRef;

	std::normal_distribution<double> *m_rndn;
};

#endif // MAINWINDOW_H
