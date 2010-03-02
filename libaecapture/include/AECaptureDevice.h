#pragma once

#include <opencv/cv.h>

#ifdef MACOSX
#import <QTKit/QTKit.h>

//QT = QuickTime here, not Qt.
@interface QTCapture : NSObject {
	//Private members
	QTCaptureSession *captureSession;
	QTCaptureDeviceInput *captureDeviceInput;
	QTCaptureDecompressedVideoOutput *captureVideoOutput;
	IplImage *frameHeader;
	IplImage *frameBuffer;
	BOOL initialized;
}
//Frame should be released by caller
-(IplImage*)latestFrame;
@end
#endif

#ifdef LINUX
#include <opencv/highgui.h> //CvCapture functions are declared here for some
							//reason.
#endif

#ifdef WINDOWS
#include <opencv/highgui.h> //CvCapture functions are declared here for some
							//reason.
#endif

namespace Aetherspark
{
	namespace Capture
	{
		class AECaptureDevice
		{
			public:
				AECaptureDevice();
				~AECaptureDevice();
				
				//The returned image should be released by the caller.
				IplImage* captureFrame();
				
			private:
				#ifdef MACOSX
				QTCapture *_capture;
				#endif
				
				#ifdef LINUX
				CvCapture *_capture;
				#endif
				
				#ifdef WINDOWS
				CvCapture *_capture;
				#endif
		};
	}
}