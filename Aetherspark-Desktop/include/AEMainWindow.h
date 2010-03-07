#pragma once

//Comment out to use the standard pixmap-based rendering
#define USE_OPENGL_VIDEO

#include <QWidget>
#include <QMainWindow>
#include <QVBoxLayout>
#ifdef USE_OPENGL_VIDEO
#include "AEGLVideoWidget.h"
#else
#include "AEVideoWidget.h"
#endif

#include "AEDllDefines.h"

namespace Aetherspark
{
	namespace Desktop
	{
		class aedesktop_EXPORT AEMainWindow : public QMainWindow
		{
			public:
				AEMainWindow(QWidget *parent = NULL);
		
			private:
				//Qt GUI controls
				#ifdef USE_OPENGL_VIDEO
				AEGLVideoWidget *_video;
				#else
				AEVideoWidget *_video;
				#endif
		};
	}
}

