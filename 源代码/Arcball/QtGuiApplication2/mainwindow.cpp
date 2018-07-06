#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QPushButton>
#include <QFileDialog>
#include <QString>
#include "glwidget.h"
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent)
{
	//ui->setupUi(this);
	QMenuBar *menuBar = new QMenuBar;
	QMenu *menuFile = menuBar->addMenu(tr("&File"));
	QMenu *menuEdit = menuBar->addMenu(tr("&Edit"));
	QAction *open = new QAction("Open", this);
	QAction *showNormal = new QAction("showNormal", this);
	menuFile->addAction(open);
	menuEdit->addAction(showNormal);
	setMenuBar(menuBar);
	GLWidget * glWidget = new GLWidget(this);
	connect(open, SIGNAL(triggered()), this, SLOT(openclicked()));
	connect(this, SIGNAL(func(QString)), glWidget, SLOT(loadFile(QString)));
	QObject::connect(showNormal, &QAction::triggered, glWidget, &GLWidget::controlNormal);
	setCentralWidget(glWidget);
}
void MainWindow::openFile() {
	emit func(this->fileName);
}
void MainWindow::openclicked()
{
	this->fileName = QFileDialog::getOpenFileName(this, tr("Open"), QString(), tr("OBJdocument(*.obj);"));
	if (!this->fileName.isEmpty()) {
		openFile();
	}
}
MainWindow::~MainWindow()
{
	delete ui;
}
