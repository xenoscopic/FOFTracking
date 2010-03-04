#pragma once

#include <opencv/cv.h>

#ifdef LINUX
#include <opencv/highgui.h> //CvCapture functions are declared here for some
							//reason.
#endif

#ifdef WINDOWS
#include <opencv/highgui.h> //CvCapture functions are declared here for some
							//reason.
#endif

#include "AEDllDefines.h"

namespace Aetherspark
{
	namespace Capture
	{
		class aecapture_EXPORT AECaptureDevice
		{
			public:
				AECaptureDevice();
				~AECaptureDevice();
				
				//The returned image should be released by the caller.
				//It will be returned in BGR format, just like with highgui.
				IplImage* captureFrame();
				
			private:
				#ifdef MACOSX
				void *_capture; //This needs to be converted to a QTCapture
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