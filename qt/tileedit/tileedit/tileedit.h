#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

private slots:
    void openDir();
    void onResolutionChanged(int val);
    void onLatidxChanged(int val);
    void onLngidxChanged(int val);
    void onLayerType0(int);
    void onLayerType1(int);
    void onLayerType2(int);
    void OnTileChangedFromPanel(int lvl, int ilat, int ilng);
	void OnTileEntered(TileCanvas *canvas);
	void OnMouseMovedInCanvas(int canvasIdx, QMouseEvent *event);

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

    std::string rootDir;
    int m_lvl;
    int m_ilat;
    int m_ilng;

    SurfTile *m_stile;
	MaskTile *m_mtile;
	NightlightTile *m_ltile;
	ElevTile *m_etile;
};

#endif // MAINWINDOW_H
