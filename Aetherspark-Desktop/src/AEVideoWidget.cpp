#include "AEVideoWidget.h"

using namespace Aetherspark::Desktop;
using namespace Aetherspark::Capture;
using namespace Aetherspark::ImageProcessing;

AEVideoWidget::AEVideoWidget(QWidget *parent) : QWidget(parent),
_pipeline(NULL)
{
	//Set up the UI
	_layout = new QVBoxLayout();
	_imageLabel = new QLabel();
	_layout->addWidget(_imageLabel); //Set up the layout, layout takes ownership
	setLayout(_layout); //Parent will take ownership of layout
	
	//Set up the capture
	_capture = new AECaptureDevice(); //TODO: Add camera selection
	
	//Create shared image buffers
	IplImage *sampleFrame = _capture->captureFrame();
	_image = QImage(QSize(sampleFrame->width, sampleFrame->height), 
					QImage::Format_RGB888);
	cvReleaseImage(&sampleFrame);
	_cvImage = cvCreateImageHeader(cvSize(_image.width(), _image.height()),
									IPL_DEPTH_8U,
									3);
	_cvImage->imageData = (char*)_image.bits();
	
	//Set up timer (parent will destroy)
	_updateTimer = new QTimer(this);
	connect(_updateTimer, SIGNAL(timeout()), this, SLOT(updateImage()));
	_updateTimer->start(50);
}

AEVideoWidget::~AEVideoWidget()
{
	//Release header
	cvReleaseImageHeader(&_cvImage);
	
	//Get rid of the capture
	delete _capture;
}

AEImageProcessingPipeline* AEVideoWidget::pipeline()
{
	return _pipeline;
}

void AEVideoWidget::setPipeline(AEImageProcessingPipeline *pipeline)
{
	if(_pipeline != NULL)
	{
		delete _pipeline;
	}
	_pipeline = pipeline;
}

void AEVideoWidget::updateImage()
{
	//Grab the new frame
	IplImage *frame = _capture->captureFrame();
	
	//Pass through the pipeline if necessary
	IplImage *copy;
	if(_pipeline != NULL)
	{
		copy = _pipeline->processImage(frame);
		cvReleaseImage(&frame);
	}
	else
	{
		copy = frame;
	}

	//Copy to our buffer
	cvCopy(copy, _cvImage, 0);

	//Get rid of the copy
	cvReleaseImage(&copy);
	
	//Convert color space ordering
	cvCvtColor(_cvImage, _cvImage, CV_BGR2RGB);
	
	//Redraw
	_imageLabel->setPixmap(QPixmap::fromImage(_image));
}


