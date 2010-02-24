#include <iostream>
#include <QApplication>
#include "AEMainWindow.h"

using namespace std;
using namespace Aetherspark::Desktop;

int main(int argc, char **argv)
{
	cout << "Aetherspark-Desktop Application" << endl;
	
	QApplication app(argc, argv);
	AEMainWindow *mainWindow = new AEMainWindow();
	mainWindow->setWindowTitle("QT OpenCV Sandbox");
	mainWindow->show();
	
	return app.exec();
}

