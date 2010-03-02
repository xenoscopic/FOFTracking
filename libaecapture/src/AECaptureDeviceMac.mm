#import "AECaptureDevice.h"

using namespace Aetherspark::Capture;

//QT = QuickTime here, not Qt.
@interface QTCapture : NSObject {
	//Private members
	QTCaptureSession* captureSession;
	QTCaptureDeviceInput* captureDeviceInput;
	QTCaptureDecompressedVideoOutput* captureVideoOutput;
	IplImage* frameImage;
}

@end

IplImage* AECaptureDevice::captureFrame()
{
	
}

