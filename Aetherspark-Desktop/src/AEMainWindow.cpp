#include "AEMainWindow.h"

using namespace Aetherspark::Desktop;

AEMainWindow::AEMainWindow(QWidget *parent) : QMainWindow(parent)
{
	_layout = new QVBoxLayout();
	_video = new AEVideoWidget();
	_layout->addWidget(_video);
	setLayout(_layout);
	resize(500, 400);
}
