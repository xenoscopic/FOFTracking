#include "AECaptureDevice.h"

using namespace Aetherspark::Capture;

AECaptureDevice::AECaptureDevice()
{
	//Set up the capture
	_capture = cvCaptureFromCAM(0);
}

AECaptureDevice::~AECaptureDevice()
{
	//Get rid of the capture
	cvReleaseCapture(&_capture);
}

IplImage* AECaptureDevice::captureFrame()
{
	return cvQueryFrame(_capture);
}

