#include "tileedit.h"
#include "ui_tileedit.h"
#include "tile.h"
#include "elevtile.h"
#include "dlgconfig.h"
#include "dlgelevconfig.h"
#include <random>

#include "QFileDialog"
#include "QResizeEvent"
#include "QMessageBox"

static std::vector<std::pair<int, int> > paintStencil1 = { {0,0} };
static std::vector<std::pair<int, int> > paintStencil2 = { {0,0}, {1,0}, {0,1}, {1,1} };
static std::vector<std::pair<int, int> > paintStencil3 = { {-1,-1}, {0,-1}, {1,-1}, {-1,0}, {0,0}, {1,0}, {-1,1}, {0,1}, {1,1} };
static std::vector<std::pair<int, int> > paintStencil4 = { {0,-1}, {1,-1}, {-1,0}, {0,0}, {1,0}, {2,0}, {-1,1}, {0,1}, {1,1}, {2,1}, {0,2}, {1,2} };
static std::vector<std::pair<int, int> > paintStencil5 = { {-1,-2},{0,-2}, {1,-2}, {-2,-1},{-1,-1}, {0,-1}, {1,-1}, {2,-1}, {-2,0}, {-1,0},{0,0}, {1,0}, {2,0}, {-2,1},{-1,1},{0,1},{1,1},{2,1}, {-1,2}, {0,2},{1,2} };
static std::vector<std::vector<std::pair<int, int>>*> paintStencil = { &paintStencil1, &paintStencil2, &paintStencil3, &paintStencil4, &paintStencil5 };

std::default_random_engine generator;

tileedit::tileedit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::tileedit)
{
    m_stile = 0;
	m_mtile = 0;
	m_ltile = 0;
	m_etile = 0;
	m_etileRef = 0;

	m_mgrSurf = 0;
	m_mgrMask = 0;
	m_mgrElev = 0;
	m_mgrElevMod = 0;

	m_openMode = TILESEARCH_CACHE | TILESEARCH_ARCHIVE;
	Tile::setOpenMode(m_openMode);

	m_mouseDown = false;
	m_rndn = 0;

	m_dlgElevConfig = 0;

    ui->setupUi(this);

    m_panel[0].canvas = ui->widgetCanvas0;
	m_panel[0].colourscale = ui->widgetColourscale0;
	m_panel[0].rangeMin = ui->labelRangeMin0;
	m_panel[0].rangeMax = ui->labelRangeMax0;
	m_panel[0].layerType = ui->comboLayerType0;
	m_panel[0].fileId = ui->labelFileId0;

	m_panel[1].canvas = ui->widgetCanvas1;
	m_panel[1].colourscale = ui->widgetColourscale1;
	m_panel[1].rangeMin = ui->labelRangeMin1;
	m_panel[1].rangeMax = ui->labelRangeMax1;
	m_panel[1].layerType = ui->comboLayerType1;
	m_panel[1].fileId = ui->labelFileId1;

    m_panel[2].canvas = ui->widgetCanvas2;
	m_panel[2].colourscale = ui->widgetColourscale2;
	m_panel[2].rangeMin = ui->labelRangeMin2;
	m_panel[2].rangeMax = ui->labelRangeMax2;
	m_panel[2].layerType = ui->comboLayerType2;
	m_panel[2].fileId = ui->labelFileId2;

    createActions();
    createMenus();

    m_lvl = 1;
    m_ilat = 0;
    m_ilng = 0;

    connect(ui->spinResolution, SIGNAL(valueChanged(int)), this, SLOT(onResolutionChanged(int)));
    connect(ui->spinLatidx, SIGNAL(valueChanged(int)), this, SLOT(onLatidxChanged(int)));
    connect(ui->spinLngidx, SIGNAL(valueChanged(int)), this, SLOT(onLngidxChanged(int)));

	connect(ui->bgrpAction, SIGNAL(buttonClicked(int)), this, SLOT(onActionButtonClicked(int)));
	ui->bgrpAction->setId(ui->btnActionNavigate, 0);
	ui->bgrpAction->setId(ui->btnActionElevEdit, 1);
	connect(ui->bgrpEdit, SIGNAL(buttonClicked(int)), this, SLOT(onEditButtonClicked(int)));
	ui->bgrpEdit->setId(ui->btnElevPaint, 0);
	ui->bgrpEdit->setId(ui->btnElevRandom, 1);
	ui->bgrpEdit->setId(ui->btnElevErase, 2);

	m_actionMode = ACTION_NAVIGATE;
	m_elevEditMode = ELEVEDIT_PAINT;
	setToolOptions();

	ui->widgetElevEditTools->setVisible(false);

	for (int i = 0; i < 3; i++) {
		m_panel[i].canvas->setIdx(i);
		connect(m_panel[i].canvas, SIGNAL(tileChanged(int, int, int)), this, SLOT(OnTileChangedFromPanel(int, int, int)));
		connect(m_panel[i].canvas, SIGNAL(tileEntered(TileCanvas*)), this, SLOT(OnTileEntered(TileCanvas*)));
		connect(m_panel[i].canvas, SIGNAL(tileLeft(TileCanvas*)), this, SLOT(OnTileLeft(TileCanvas*)));
		connect(m_panel[i].canvas, SIGNAL(mouseMovedInCanvas(int, QMouseEvent*)), this, SLOT(OnMouseMovedInCanvas(int, QMouseEvent*)));
		connect(m_panel[i].canvas, SIGNAL(mousePressedInCanvas(int, QMouseEvent*)), this, SLOT(OnMousePressedInCanvas(int, QMouseEvent*)));
		connect(m_panel[i].canvas, SIGNAL(mouseReleasedInCanvas(int, QMouseEvent*)), this, SLOT(OnMouseReleasedInCanvas(int, QMouseEvent*)));
		m_panel[i].colourscale->setVisible(false);
		m_panel[i].colourscale->findChild<Colorbar*>()->setElevDisplayParam(m_elevDisplayParam);
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

	releaseTreeManagers();
}

void tileedit::createMenus()
{
    fileMenu = ui->menuBar->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
	fileMenu->addSeparator();
	fileMenu->addAction(actionConfig);
	fileMenu->addSeparator();
	fileMenu->addAction(actionExit);

	QMenu *menu = ui->menuBar->addMenu(tr("&Elevation"));
	menu->addAction(actionElevConfig);
}

void tileedit::createActions()
{
    openAct = new QAction(tr("&Open"), this);
    connect(openAct, &QAction::triggered, this, &tileedit::openDir);

	actionConfig = new QAction(tr("&Configure"), this);
	connect(actionConfig, &QAction::triggered, this, &tileedit::on_actionConfig_triggered);

	actionExit = new QAction(tr("E&xit"), this);
	connect(actionExit, &QAction::triggered, this, &tileedit::on_actionExit_triggered);

	actionElevConfig = new QAction(tr("&Configure"), this);
	actionElevConfig->setCheckable(true);
	connect(actionElevConfig, &QAction::triggered, this, &tileedit::onElevConfig);
}

void tileedit::elevDisplayParamChanged()
{
	char cbuf[1024];

	if (m_etile) {
		if (m_elevDisplayParam.autoRange) {
			m_elevDisplayParam.rangeMin = m_etile->getData().dmin;
			m_elevDisplayParam.rangeMax = m_etile->getData().dmax;
		}

		m_etile->displayParamChanged();
	}

	for (int i = 0; i < 3; i++) {
		if (m_panel[i].layerType->currentIndex() == 3)
			m_panel[i].canvas->update();
		Colorbar *cbar = m_panel[i].colourscale->findChild<Colorbar*>();
		if (cbar) {
			cbar->displayParamChanged();
		}
		if (m_elevDisplayParam.autoRange) {
			if (m_etile) {
				sprintf(cbuf, "%+0.1f", m_etile->getData().dmin);
				m_panel[i].rangeMin->setText(cbuf);
				sprintf(cbuf, "%+0.1f", m_etile->getData().dmax);
				m_panel[i].rangeMax->setText(cbuf);
			}
		}
		else {
			sprintf(cbuf, "%+0.1f", m_elevDisplayParam.rangeMin);
			m_panel[i].rangeMin->setText(cbuf);
			sprintf(cbuf, "%+0.1f", m_elevDisplayParam.rangeMax);
			m_panel[i].rangeMax->setText(cbuf);
		}
	}
}

void tileedit::setLoadMode(DWORD mode)
{
	if (mode != m_openMode) {
		m_openMode = mode;
		Tile::setOpenMode(m_openMode);

		// reload current tile
		setTile(m_lvl, m_ilat, m_ilng);
	}
}

void tileedit::openDir()
{
    std::string rootDir = QFileDialog::getExistingDirectory(this, tr("Open celestial body")).toStdString();
	Tile::setRoot(rootDir);

	if (m_openMode & TILESEARCH_ARCHIVE)
		setupTreeManagers(rootDir);

    setTile(1, 0, 0);
    ensureSquareCanvas(rect().width(), rect().height());

	char cbuf[256];
	sprintf(cbuf, "tileedit [%s]", rootDir.c_str());
	setWindowTitle(cbuf);
}

void tileedit::on_actionConfig_triggered()
{
	DlgConfig dlg(this);
	dlg.exec();
}

void tileedit::on_actionExit_triggered()
{
	QApplication::quit();
}

void tileedit::onElevConfig()
{
	if (!m_dlgElevConfig) {
		m_dlgElevConfig = new DlgElevConfig(this, m_elevDisplayParam);
		connect(m_dlgElevConfig, SIGNAL(finished(int)), this, SLOT(onElevConfigDestroyed(int)));
		m_dlgElevConfig->show();
		actionElevConfig->setChecked(true);
	}
	else
		onElevConfigDestroyed(0);
}

void tileedit::onElevConfigDestroyed(int r)
{
	if (m_dlgElevConfig) {
		delete m_dlgElevConfig;
		m_dlgElevConfig = 0;
		actionElevConfig->setChecked(false);
	}
}

void tileedit::loadTile(int lvl, int ilat, int ilng)
{
    if (m_stile)
        delete m_stile;
    m_stile = SurfTile::Load(lvl, ilat, ilng);

	if (m_mtile)
		delete m_mtile;
	if (m_ltile)
		delete m_ltile;
	std::pair<MaskTile*, NightlightTile*> ml = MaskTile::Load(lvl, ilat, ilng);
	m_mtile = ml.first;
	m_ltile = ml.second;

	if (m_etile)
		delete m_etile;
	m_etile = ElevTile::Load(lvl, ilat, ilng, m_elevDisplayParam, &cmap(m_elevDisplayParam.cmName));
	if (m_etile && m_mtile)
		m_etile->setWaterMask(m_mtile);

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
		elevDisplayParamChanged();
		break;
	default:
        m_panel[panelIdx].canvas->setImage(0);
        break;
    }
	m_panel[panelIdx].colourscale->setVisible(m_panel[panelIdx].layerType->currentIndex() == 3);
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

void tileedit::onActionButtonClicked(int id)
{
	ActionMode newMode = (ActionMode)id;
	if (newMode == m_actionMode)
		return;

	if (newMode == ACTION_ELEVEDIT) {
		if (!m_etile) {
			ui->btnActionNavigate->setChecked(true);
			return;
		}
		if (m_etile->subLevel() < m_etile->Level()) {
			QMessageBox mbox(QMessageBox::Warning, "tileedit", "This elevation tile does not exist - the image you see has been synthesized from a subsection of an ancestor.\n\nBefore this tile can be edited it must be created. Do you want to create it now?", QMessageBox::Yes | QMessageBox::No);
			mbox.exec();
			ui->btnActionNavigate->setChecked(true);
			return; // don't allow editing virtual tiles
		}
	}

	m_actionMode = newMode;
	ui->widgetElevEditTools->setVisible(id == 1);
	ui->gboxToolOptions->setTitle(ModeString());
	setToolOptions();
	for (int i = 0; i < 3; i++)
		m_panel[i].canvas->setGlyphMode((TileCanvas::GlyphMode)id);
}

void tileedit::onEditButtonClicked(int id)
{
	m_elevEditMode = (ElevEditMode)id;
	ui->gboxToolOptions->setTitle(ModeString());
	setToolOptions();
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
	for (int i = 0; i < 3; i++) {
		if (m_actionMode != ACTION_NAVIGATE || m_panel[i].canvas == canvas) {
			m_panel[i].canvas->blockSignals(true);
			m_panel[i].canvas->showOverlay(true);
			m_panel[i].canvas->blockSignals(false);
		}
	}

	int idx = -1;
	const Tile *tile = canvas->tile();
	if (tile) idx = canvas->idx();

	ui->groupTileData->setTitle(idx >= 0 ? m_panel[idx].layerType->currentText() : "Tile");
	ui->labelKey2->setText(idx >= 0 && m_panel[idx].layerType->currentIndex() == 3 ? "Node:" : "Pixel:");

	const char *ValStr[4] = { "Colour:", "Type:", "Colour:", "Elev.:" };
	ui->labelKey3->setText(idx >= 0 ? ValStr[m_panel[idx].layerType->currentIndex()] : "-");
}

void tileedit::OnTileLeft(TileCanvas *canvas)
{
	for (int i = 0; i < 3; i++) {
		if (m_actionMode != ACTION_NAVIGATE || m_panel[i].canvas == canvas) {
			m_panel[i].canvas->blockSignals(true);
			m_panel[i].canvas->showOverlay(false);
			m_panel[i].canvas->blockSignals(false);
		}
	}
}

void tileedit::OnMouseMovedInCanvas(int canvasIdx, QMouseEvent *event)
{
	const Tile *tile = m_panel[canvasIdx].canvas->tile();
	int x = event->x();
	int y = event->y();
	int cw = m_panel[canvasIdx].canvas->rect().width();
	int ch = m_panel[canvasIdx].canvas->rect().height();

	if (tile && tile->getImage().data.size()) {
		int iw = tile->getImage().width;
		int ih = tile->getImage().height;

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
			m_panel[canvasIdx].colourscale->findChild<Colorbar*>()->setValue(elev);
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
		for (int i = 0; i < 3; i++) {
			if (m_etile && m_panel[i].layerType->currentIndex() == 3) {
				iw = m_etile->getImage().width;
				ih = m_etile->getImage().height;
				mx = ((x*iw) / cw + 1) / 2;
				my = (ih - (y*ih) / ch) / 2;
				double elev = m_etile->nodeElevation(mx, my);
				m_panel[i].colourscale->findChild<Colorbar*>()->setValue(elev);
			}
		}
	}
	else {
		ui->labelData1->setText("-");
		ui->labelData2->setText("-");
	}

	if (m_actionMode == ACTION_ELEVEDIT && m_etile) {
		if (m_mouseDown)
			editElevation(canvasIdx, event->x(), event->y());
		for (int i = 0; i < 3; i++) {
			m_panel[i].canvas->setCrosshair(((double)x + 0.5) / (double)cw, ((double)y + 0.5) / (double)ch);
		}
	}
}

std::pair<int, int> tileedit::ElevNodeFromPixCoord(int canvasIdx, int x, int y)
{
	std::pair<int, int> node;
	const Tile *tile = m_panel[canvasIdx].canvas->tile();
	if (tile && tile->getImage().data.size()) {
		int iw = tile->getImage().width;
		int ih = tile->getImage().height;
		int cw = m_panel[canvasIdx].canvas->rect().width();
		int ch = m_panel[canvasIdx].canvas->rect().height();
		int mx = (x*iw) / cw;
		int my = (y*ih) / ch;
		mx = (mx + 1) / 2;
		my = (ih - my) / 2;
		node.first = mx;
		node.second = my;
	}
	return node;
}

void tileedit::OnMousePressedInCanvas(int canvasIdx, QMouseEvent *event)
{
	if (m_actionMode == ACTION_ELEVEDIT && m_etile) {
		if (m_etileRef)
			delete m_etileRef;
		m_etileRef = new ElevTile(*m_etile);
		if (m_elevEditMode == ELEVEDIT_RANDOM) {
			double mean = (double)ui->spinElevRandomValue->value();
			double std = ui->dspinElevRandomStd->value();
			m_rndn = new std::normal_distribution<double>(mean, std);
		}
		editElevation(canvasIdx, event->x(), event->y());
	}
	m_mouseDown = true;
}

void tileedit::OnMouseReleasedInCanvas(int canvasIdx, QMouseEvent *event)
{
	m_mouseDown = false;
	if (m_etileRef) {
		delete m_etileRef;
		m_etileRef = 0;
	}
	if (m_rndn) {
		delete m_rndn;
		m_rndn = 0;
	}
}

void tileedit::editElevation(int canvasIdx, int x, int y)
{
	std::pair<int, int> elevcrd = ElevNodeFromPixCoord(canvasIdx, x, y);
	int nx = elevcrd.first;
	int ny = elevcrd.second;
	const int padx = 1;
	const int pady = 1;

	switch (m_elevEditMode) {
	case ELEVEDIT_PAINT:
	case ELEVEDIT_RANDOM:
		{
			int v = ui->spinElevPaintValue->value();
			ElevData &edata = m_etile->getData();
			int sz = (m_elevEditMode == ELEVEDIT_PAINT ?
				ui->spinElevPaintSize->value() :
				ui->spinElevRandomSize->value()
				);
			sz = min(sz, 5);
			int mode = (m_elevEditMode == ELEVEDIT_PAINT ?
				ui->comboElevPaintMode->currentIndex() :
				ui->comboElevRandomMode->currentIndex()
				);
			std::vector<std::pair<int, int>> *stencil = paintStencil[sz - 1];
			bool ismod = false;
			bool boundsChanged = false;
			bool rescanBounds = false;
			for (int i = 0; i < stencil->size(); i++) {
				if (m_elevEditMode == ELEVEDIT_RANDOM)
					v = (int)(*m_rndn)(generator);
				int idx = (ny + pady + (*stencil)[i].second)*edata.width + (nx + padx + (*stencil)[i].first);
				if (idx >= 0 && idx < edata.data.size()) {
					INT16 vold = edata.data[idx];
					switch (mode) {
					case 0:
						edata.data[idx] = (INT16)v;
						break;
					case 1:
						edata.data[idx] = m_etileRef->getData().data[idx] + (INT16)v;
						break;
					case 2:
						if (edata.data[idx] < (INT16)v)
							edata.data[idx] = (INT16)v;
						break;
					case 3:
						if (edata.data[idx] > (INT16)v)
							edata.data[idx] = (INT16)v;
						break;
					}
					if (edata.data[idx] != vold) {
						if (vold == edata.dmin && edata.data[idx] > vold ||
							vold == edata.dmax && edata.data[idx] < vold) {
							rescanBounds = true;
						}
						else {
							if (edata.data[idx] < edata.dmin) {
								edata.dmin = edata.data[idx];
								boundsChanged = true;
							}
							if (edata.data[idx] > edata.dmax) {
								edata.dmax = edata.data[idx];
								boundsChanged = true;
							}
						}
						ismod = true;
					}
				}
			}
			if (ismod) {
				if (rescanBounds) {
					edata.dmin = *std::min_element(edata.data.begin(), edata.data.end());
					edata.dmax = *std::max_element(edata.data.begin(), edata.data.end());
					boundsChanged = true;
				}
				if (boundsChanged)
					m_etile->dataChanged();
				else
					m_etile->dataChanged(nx + padx - (sz / 2 + 1), nx + padx + (sz / 2 + 1), ny + pady - (sz / 2 + 1), ny + pady + (sz / 2 + 1));
				for (int i = 0; i < 3; i++)
					if (m_panel[i].layerType->currentIndex() == 3) { // elevation
						refreshPanel(i);
					}
			}
		}
		break;
	case ELEVEDIT_ERASE:
		{
			int sz = ui->spinElevEraseSize->value();
			sz = min(sz, 5);
			std::vector<std::pair<int, int>> *stencil = paintStencil[sz - 1];
			ElevData &edata = m_etile->getData();
			ElevData &edataBase = m_etile->getBaseData();
			bool ismod = false;
			bool boundsChanged = false;
			for (int i = 0; i < stencil->size(); i++) {
				int idx = (ny + pady + (*stencil)[i].second)*edata.width + (nx + padx + (*stencil)[i].first);
				if (idx >= 0 && idx < edata.data.size() && edata.data[idx] != edataBase.data[idx]) {
					if (edata.data[idx] == edata.dmin || edata.data[idx] == edata.dmax)
						boundsChanged = true;
					edata.data[idx] = edataBase.data[idx];
					ismod = true;
				}
			}
			if (ismod) {
				if (boundsChanged) {
					edata.dmin = *std::min_element(edata.data.begin(), edata.data.end());
					edata.dmax = *std::max_element(edata.data.begin(), edata.data.end());
					m_etile->dataChanged();
				} else
					m_etile->dataChanged(nx + padx - (sz / 2 + 1), nx + padx + (sz / 2 + 1), ny + pady - (sz / 2 + 1), ny + pady + (sz / 2 + 1));
				for (int i = 0; i < 3; i++)
					if (m_panel[i].layerType->currentIndex() == 3) { // elevation
						refreshPanel(i);
					}
			}
		}
		break;
	}
}

QString tileedit::ModeString() const
{
	std::string actionStr[2] = { "Navigate" , "Elevation: " };
	std::string eleveditStr[3] = { "Paint value", "Paint random", "Erase" };
	QString str(QString::fromStdString(actionStr[m_actionMode]));
	if (m_actionMode == 1) {
		str.append(QString::fromStdString(eleveditStr[m_elevEditMode]));
	}
	return str;
}

void tileedit::setToolOptions()
{
	ui->widgetElevPaint->setVisible(m_actionMode == ACTION_ELEVEDIT && m_elevEditMode == ELEVEDIT_PAINT);
	ui->widgetElevRandom->setVisible(m_actionMode == ACTION_ELEVEDIT && m_elevEditMode == ELEVEDIT_RANDOM);
	ui->widgetElevErase->setVisible(m_actionMode == ACTION_ELEVEDIT && m_elevEditMode == ELEVEDIT_ERASE);
}

void tileedit::setTile(int lvl, int ilat, int ilng)
{
	if (m_etile && m_etile->isModified()) {
		m_etile->MatchNeighbourTiles();
		m_etile->SaveMod();
		m_etile->MatchParentTile(m_etile->Level() - 5);
	}

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

void tileedit::setupTreeManagers(std::string &root)
{
	releaseTreeManagers();

	m_mgrSurf = ZTreeMgr::CreateFromFile(root.c_str(), ZTreeMgr::LAYER_SURF);
	SurfTile::setTreeMgr(m_mgrSurf);

	m_mgrMask = ZTreeMgr::CreateFromFile(root.c_str(), ZTreeMgr::LAYER_MASK);
	MaskTile::setTreeMgr(m_mgrMask);

	m_mgrElev = ZTreeMgr::CreateFromFile(root.c_str(), ZTreeMgr::LAYER_ELEV);
	m_mgrElevMod = ZTreeMgr::CreateFromFile(root.c_str(), ZTreeMgr::LAYER_ELEVMOD);
	ElevTile::setTreeMgr(m_mgrElev, m_mgrElevMod);
}

void tileedit::releaseTreeManagers()
{
	if (m_mgrSurf) {
		delete m_mgrSurf;
		m_mgrSurf = 0;
	}
	if (m_mgrMask) {
		delete m_mgrMask;
		m_mgrMask = 0;
	}
	if (m_mgrElev) {
		delete m_mgrElev;
		m_mgrElev = 0;
	}
	if (m_mgrElevMod) {
		delete m_mgrElevMod;
		m_mgrElevMod = 0;
	}
}
