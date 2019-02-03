#include "tileedit.h"
#include "ui_tileedit.h"
#include "tile.h"

#include "QFileDialog"
#include "QResizeEvent"

tileedit::tileedit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tileedit)
{
    m_stile = 0;
	m_mtile = 0;
	m_ltile = 0;
	m_etile = 0;

    ui->setupUi(this);

    m_panel[0].canvas = ui->widgetCanvas0;
    m_panel[1].canvas = ui->widgetCanvas1;
    m_panel[2].canvas = ui->widgetCanvas2;
    m_panel[0].layerType = ui->comboLayerType0;
    m_panel[1].layerType = ui->comboLayerType1;
    m_panel[2].layerType = ui->comboLayerType2;
	m_panel[0].fileId = ui->labelFileId0;
	m_panel[1].fileId = ui->labelFileId1;
	m_panel[2].fileId = ui->labelFileId2;

    createActions();
    createMenus();

    m_lvl = 1;
    m_ilat = 0;
    m_ilng = 0;

    connect(ui->spinResolution, SIGNAL(valueChanged(int)), this, SLOT(onResolutionChanged(int)));
    connect(ui->spinLatidx, SIGNAL(valueChanged(int)), this, SLOT(onLatidxChanged(int)));
    connect(ui->spinLngidx, SIGNAL(valueChanged(int)), this, SLOT(onLngidxChanged(int)));

	for (int i = 0; i < 3; i++) {
		m_panel[i].canvas->setIdx(i);
		connect(m_panel[i].canvas, SIGNAL(tileChanged(int, int, int)), this, SLOT(OnTileChangedFromPanel(int, int, int)));
		connect(m_panel[i].canvas, SIGNAL(tileEntered(TileCanvas*)), this, SLOT(OnTileEntered(TileCanvas*)));
		connect(m_panel[i].canvas, SIGNAL(mouseMovedInCanvas(int, QMouseEvent*)), this, SLOT(OnMouseMovedInCanvas(int, QMouseEvent*)));
	}
    connect(m_panel[0].layerType, SIGNAL(currentIndexChanged(int)), this, SLOT(onLayerType0(int)));
    connect(m_panel[1].layerType, SIGNAL(currentIndexChanged(int)), this, SLOT(onLayerType1(int)));
    connect(m_panel[2].layerType, SIGNAL(currentIndexChanged(int)), this, SLOT(onLayerType2(int)));

    ui->statusBar->addWidget(status = new QLabel());
}

tileedit::~tileedit()
{
    delete ui;
    if (m_stile)
        delete m_stile;
	if (m_mtile)
		delete m_mtile;
	if (m_ltile)
		delete m_ltile;
	if (m_etile)
		delete m_etile;
}

void tileedit::createMenus()
{
    fileMenu = ui->menuBar->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
}

void tileedit::createActions()
{
    openAct = new QAction(tr("&Open"), this);
    connect(openAct, &QAction::triggered, this, &tileedit::openDir);
}

void tileedit::openDir()
{
    rootDir = QFileDialog::getExistingDirectory(this, tr("Open celestial body")).toStdString();
    setTile(1, 0, 0);
    ensureSquareCanvas(rect().width(), rect().height());

	char cbuf[256];
	sprintf(cbuf, "tileedit [%s]", rootDir.c_str());
	setWindowTitle(cbuf);
}

void tileedit::loadTile(int lvl, int ilat, int ilng)
{
    if (m_stile)
        delete m_stile;
    m_stile = SurfTile::Load(rootDir, lvl, ilat, ilng);

	if (m_mtile)
		delete m_mtile;
	if (m_ltile)
		delete m_ltile;
	std::pair<MaskTile*, NightlightTile*> ml = MaskTile::Load(rootDir, lvl, ilat, ilng);
	m_mtile = ml.first;
	m_ltile = ml.second;

	if (m_etile)
		delete m_etile;
	m_etile = ElevTile::Load(rootDir, lvl, ilat, ilng);

    for (int i = 0; i < 3; i++)
        refreshPanel(i);
}

void tileedit::refreshPanel(int panelIdx)
{
	char cbuf[256];

    switch(m_panel[panelIdx].layerType->currentIndex()) {
    case 0:
        m_panel[panelIdx].canvas->setImage(m_stile);
		if (m_stile)
			sprintf(cbuf, "%02d / %06d / %06d", m_stile->subLevel(), m_stile->subiLat(), m_stile->subiLng());
		else
			cbuf[0] = '\0';
		m_panel[panelIdx].fileId->setText(cbuf);
        break;
	case 1:
		m_panel[panelIdx].canvas->setImage(m_mtile);
		if (m_mtile)
			sprintf(cbuf, "%02d / %06d / %06d", m_mtile->subLevel(), m_mtile->subiLat(), m_mtile->subiLng());
		else
			cbuf[0] = '\0';
		m_panel[panelIdx].fileId->setText(cbuf);
		break;
	case 2:
		m_panel[panelIdx].canvas->setImage(m_ltile);
		if (m_ltile)
			sprintf(cbuf, "%02d / %06d / %06d", m_ltile->subLevel(), m_ltile->subiLat(), m_ltile->subiLng());
		else
			cbuf[0] = '\0';
		m_panel[panelIdx].fileId->setText(cbuf);
		break;
	case 3:
		m_panel[panelIdx].canvas->setImage(m_etile);
		if (m_etile)
			sprintf(cbuf, "%02d / %06d / %06d", m_etile->subLevel(), m_etile->subiLat(), m_etile->subiLng());
		else
			cbuf[0] = '\0';
		m_panel[panelIdx].fileId->setText(cbuf);
		break;
	default:
        m_panel[panelIdx].canvas->setImage(0);
        break;
    }
}

void tileedit::onResolutionChanged(int lvl)
{
	while (lvl > m_lvl) {
		m_ilat *= 2;
		m_ilng *= 2;
		m_lvl++;
	}
	while (lvl < m_lvl) {
		m_ilat /= 2;
		m_ilng /= 2;
		m_lvl--;
	}
	setTile(m_lvl, m_ilat, m_ilng);
}

void tileedit::onLatidxChanged(int ilat)
{
	setTile(m_lvl, ilat, m_ilng);
}

void tileedit::onLngidxChanged(int ilng)
{
	setTile(m_lvl, m_ilat, ilng);
}

void tileedit::onLayerType0(int idx)
{
    refreshPanel(0);
}

void tileedit::onLayerType1(int idx)
{
    refreshPanel(1);
}

void tileedit::onLayerType2(int idx)
{
    refreshPanel(2);
}

void tileedit::resizeEvent(QResizeEvent *event)
{
    int winw = event->size().width();
    int winh = event->size().height();
	ensureSquareCanvas(winw, winh);
}

void tileedit::ensureSquareCanvas(int winw, int winh)
{
    // make canvas areas square
    int w = ui->widgetCanvas0->width();
    int h = ui->widgetCanvas0->height();
    int dw = w-h;

	if (dw) {
		blockSignals(true);
		setFixedHeight(winh + dw);
		blockSignals(false);
	}
}

void tileedit::OnTileChangedFromPanel(int lvl, int ilat, int ilng)
{
	setTile(lvl, ilat, ilng);
}

void tileedit::OnTileEntered(TileCanvas *canvas)
{
	int idx = -1;
	const Tile *tile = canvas->tile();
	if (tile) idx = canvas->idx();

	ui->groupTileData->setTitle(idx >= 0 ? m_panel[idx].layerType->currentText() : "");
	ui->labelKey2->setText(idx >= 0 && m_panel[idx].layerType->currentIndex() == 3 ? "Node:" : "Pixel:");

	const char *ValStr[4] = { "Colour:", "Surf. type:", "Colour:", "Elevation:" };
	ui->labelKey3->setText(idx >= 0 ? ValStr[m_panel[idx].layerType->currentIndex()] : "-");
}

void tileedit::OnMouseMovedInCanvas(int canvasIdx, QMouseEvent *event)
{
	const Tile *tile = m_panel[canvasIdx].canvas->tile();
	if (tile && tile->getImage().data.size()) {
		int x = event->x();
		int y = event->y();
		int iw = tile->getImage().width;
		int ih = tile->getImage().height;
		int cw = m_panel[canvasIdx].canvas->rect().width();
		int ch = m_panel[canvasIdx].canvas->rect().height();

		int nlat = (m_lvl < 4 ? 1 : 1 << (m_lvl - 4));
		int nlng = (m_lvl < 4 ? 1 : 1 << (m_lvl - 3));
		double latmax = (1.0 - (double)m_ilat / (double)nlat) * 180.0 - 90.0;
		double latmin = latmax - 180.0 / nlat;
		double lngmin = (double)m_ilng / (double)nlng * 360.0 - 180.0;
		double lngmax = lngmin + 360.0 / nlng;

		double lng = lngmin + ((double)x + 0.5) / (double)cw * (lngmax - lngmin);
		double lat = latmax - ((double)y + 0.5) / (double)ch * (latmax - latmin);

		char cbuf[1024];
		sprintf(cbuf, "Lng=%+0.6lf, Lat=%+0.6lf", lng, lat);
		ui->labelData1->setText(cbuf);

		int mx = (x*iw) / cw;
		int my = (y*ih) / ch;
		if (m_panel[canvasIdx].layerType->currentIndex() == 3) { // elevation
			mx = (mx + 1) / 2;
			my = (ih - my) / 2;
			sprintf(cbuf, "x=%d/%d, y=%d/%d", mx, (iw / 2) + 1, my, (ih / 2) + 1);
			ui->labelData2->setText(cbuf);
			double elev = ((ElevTile*)tile)->nodeElevation(mx, my);
			sprintf(cbuf, "%+0.1lfm", elev);
			ui->labelData3->setText(cbuf);
		}
		else {
			sprintf(cbuf, "X=%d/%d, Y=%d/%d", mx, iw, my, ih);
			ui->labelData2->setText(cbuf);
			DWORD col = tile->pixelColour(mx, my);
			if (m_panel[canvasIdx].layerType->currentIndex() == 1) {
				ui->labelData3->setText(col & 0xFF000000 ? "Diffuse (Land)" : "Specular (Water)");
			} else {
				sprintf(cbuf, "R=%d, G=%d, B=%d", (col >> 0x10) & 0xFF, (col >> 0x08) & 0xFF, col & 0xFF);
				ui->labelData3->setText(cbuf);
			}
		}
	}
	else {
		ui->labelData1->setText("-");
		ui->labelData2->setText("-");
	}
}

void tileedit::setTile(int lvl, int ilat, int ilng)
{
	m_lvl = lvl;
	m_ilat = ilat;
	m_ilng = ilng;
	loadTile(lvl, ilat, ilng);

	int nlat = (m_lvl < 4 ? 1 : 1 << (m_lvl - 4));
	int nlng = (m_lvl < 4 ? 1 : 1 << (m_lvl - 3));

	double latmax = (1.0 - (double)m_ilat / (double)nlat) * 180.0 - 90.0;
	double latmin = latmax - 180.0 / nlat;
	double lngmin = (double)m_ilng / (double)nlng * 360.0 - 180.0;
	double lngmax = lngmin + 360.0 / nlng;

	if (ui->spinResolution->value() != lvl) {
		ui->spinResolution->blockSignals(true);
		ui->spinResolution->setValue(lvl);
		ui->spinResolution->blockSignals(false);
	}
	if (ui->spinLatidx->maximum() != nlat - 1 || ui->spinLatidx->value() != ilat) {
		ui->spinLatidx->blockSignals(true);
		ui->spinLatidx->setMaximum(nlat - 1);
		ui->spinLatidx->setValue(ilat);
		ui->spinLatidx->blockSignals(false);
	}
	if (ui->spinLngidx->maximum() != nlng - 1 || ui->spinLngidx->value() != ilng) {
		ui->spinLngidx->blockSignals(true);
		ui->spinLngidx->setMaximum(nlng - 1);
		ui->spinLngidx->setValue(ilng);
		ui->spinLngidx->blockSignals(false);
	}
	char cbuf[256];
	sprintf(cbuf, "%+0.10lf", latmin);
	ui->labelLatmin->setText(cbuf);
	sprintf(cbuf, "%+0.10lf", latmax);
	ui->labelLatmax->setText(cbuf);
	sprintf(cbuf, "%+0.10lf", lngmin);
	ui->labelLngmin->setText(cbuf);
	sprintf(cbuf, "%+0.10lf", lngmax);
	ui->labelLngmax->setText(cbuf);
}