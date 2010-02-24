#include "AEMainWindow.h"

using namespace Aetherspark::Desktop;

AEMainWindow::AEMainWindow(QWidget *parent) : QMainWindow(parent)
{
	#ifdef USE_OPENGL_VIDEO
	_video = new AEGLVideoWidget();
	#else
	_video = new AEVideoWidget();
	#endif
	setCentralWidget(_video);
	resize(500, 400);
}

