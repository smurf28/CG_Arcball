#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow mw;
	mw.setWindowTitle("ArcBall");
	mw.show();
	return a.exec();
}
