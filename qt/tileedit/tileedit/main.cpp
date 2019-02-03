#include "tileedit.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	tileedit w;
	w.show();
	return a.exec();
}
