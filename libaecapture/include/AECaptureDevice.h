#pragma once

#include <opencv/cv.h>

namespace Aetherspark
{
	namespace Capture
	{
		class AECaptureDevice
		{
			public:
				IplImage* captureFrame();
		};
	}
}