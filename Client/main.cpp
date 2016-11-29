#include "yjchat.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	YJChat w;
	w.show();
	return a.exec();
}
