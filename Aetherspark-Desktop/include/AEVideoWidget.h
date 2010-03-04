#pragma once

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>

#include <opencv/cv.h>
#include <opencv/highgui.h> //CvCapture functions are declared here for some
							//reason.

#include "AEDllDefines.h"

namespace Aetherspark
{
	namespace Desktop
	{
		class aedesktop_EXPORT AEVideoWidget : public QWidget
		{
			Q_OBJECT
			
			public:
				AEVideoWidget(QWidget *parent = NULL);
				~AEVideoWidget();
			
			public slots:
				void updateImage();
			
			private:
				//Capture source
				CvCapture *_capture;
			
				//Our image buffers.  They actually share the same buffer, which
				//is really owned by QImage
				QImage _image;
				IplImage *_cvImage;
				
				//Qt GUI controls
				QLabel *_imageLabel;
				QVBoxLayout *_layout;
				
				//Update timer
				QTimer *_updateTimer;
		};
	}
}

