#pragma once

#include <QWidget>
#include <QGLWidget>
#include <QTimer>

#include <opencv/cv.h>
#include <opencv/highgui.h> //CvCapture functions are declared here for some
							//reason.

#define IMAGE_CACHE_SIZE 3

namespace Aetherspark
{
	namespace Desktop
	{
		struct cvTexture
		{
			GLuint texId;
			IplImage *texImage;
			short initialized;
		};
		
		class AEGLVideoWidget : public QGLWidget
		{
			Q_OBJECT
			
			public:
				AEGLVideoWidget(QWidget *parent = NULL);
				~AEGLVideoWidget();
			
			protected:
				void initializeGL();
				void resizeGL(int w, int h);
				void paintGL();
				
			private slots:
				void updateImage();
			
			private:
				void setImage(IplImage *img);
				
				//Capture source
				CvCapture *_capture;

				//Update timer
				QTimer *_updateTimer;
				
				//Image buffer data
				int _imageIndex;
				struct cvTexture _cvTextures[IMAGE_CACHE_SIZE];
				GLuint _rectList;
		};
	}
}
