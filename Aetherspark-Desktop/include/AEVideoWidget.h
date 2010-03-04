#pragma once

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>

#include <opencv/cv.h>

#include "AECaptureDevice.h"
							
#include "AEImageProcessingPipeline.h"

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

				Aetherspark::ImageProcessing::AEImageProcessingPipeline* pipeline();
				//Takes ownership of pipeline
				void setPipeline(Aetherspark::ImageProcessing::AEImageProcessingPipeline *pipeline = NULL);
			
			public slots:
				void updateImage();
			
			private:
				//Capture source
				Aetherspark::Capture::AECaptureDevice *_capture;
			
				//Our image buffers.  They actually share the same buffer, which
				//is really owned by QImage
				QImage _image;
				IplImage *_cvImage;
				
				//Qt GUI controls
				QLabel *_imageLabel;
				QVBoxLayout *_layout;
				
				//Update timer
				QTimer *_updateTimer;

				//Image pipeline
				Aetherspark::ImageProcessing::AEImageProcessingPipeline *_pipeline;
		};
	}
}

