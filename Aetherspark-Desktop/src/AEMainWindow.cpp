#include "AEMainWindow.h"

using namespace Aetherspark::Desktop;

AEMainWindow::AEMainWindow(QWidget *parent) : QMainWindow(parent)
{
	_video = new AEVideoWidget();
	setCentralWidget(_video);
	resize(500, 400);
}

