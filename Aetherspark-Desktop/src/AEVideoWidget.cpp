#include "AEVideoWidget.h"

using namespace Aetherspark::Desktop;

AEVideoWidget::AEVideoWidget(QWidget *parent) : QWidget(parent)
{
	//Set up the UI
	_layout = new QVBoxLayout();
	_imageLabel = new QLabel();
	_layout->addWidget(_imageLabel); //Set up the layout, layout takes ownership
	setLayout(_layout); //Parent will take ownership of layout
	
	//Set up the capture
	_capture = cvCaptureFromCAM(0); //TODO: Add camera selection
	
	//Create shared image buffers
	IplImage *sampleFrame = cvQueryFrame(_capture);
	_image = QImage(QSize(sampleFrame->width, sampleFrame->height), 
					QImage::Format_RGB888);
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
	cvReleaseCapture(&_capture);
}

void AEVideoWidget::updateImage()
{
	//Grab the new frame
	IplImage *frame = cvQueryFrame(_capture);
	
	//Copy to our buffer
	cvCopy(frame, _cvImage, 0);
	
	//Convert color space ordering
	cvCvtColor(_cvImage, _cvImage, CV_BGR2RGB);
	
	//Redraw
	_imageLabel->setPixmap(QPixmap::fromImage(_image));
}


