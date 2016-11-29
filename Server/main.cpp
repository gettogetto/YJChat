#include "yjchatserver.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	YJChatServer w;
	w.show();
	return a.exec();
}
