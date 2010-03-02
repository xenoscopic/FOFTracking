#pragma once

#include <QWidget>
#include <QGLWidget>
#include <QTimer>

#include <opencv/cv.h>

#include "AECaptureDevice.h"
							
#include "AEImageProcessingPipeline.h"

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
				
				Aetherspark::ImageProcessing::AEImageProcessingPipeline* pipeline();
				//Takes ownership of pipeline
				void setPipeline(Aetherspark::ImageProcessing::AEImageProcessingPipeline *pipeline = NULL);
			
			protected:
				void initializeGL();
				void resizeGL(int w, int h);
				void paintGL();
				
			private slots:
				void updateImage();
			
			private:
				void setImage(IplImage *img);
				
				//Capture source
				Aetherspark::Capture::AECaptureDevice *_capture;

				//Update timer
				QTimer *_updateTimer;
				
				//Image buffer data
				int _imageIndex;
				struct cvTexture _cvTextures[IMAGE_CACHE_SIZE];
				GLuint _rectList;
				
				//Image pipeline
				Aetherspark::ImageProcessing::AEImageProcessingPipeline *_pipeline;
		};
	}
}
