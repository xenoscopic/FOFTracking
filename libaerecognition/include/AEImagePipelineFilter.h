#pragma once

#define CV_NO_BACKWARD_COMPATIBILITY

#include <opencv/cv.h>
#include <boost/shared_ptr.hpp>

#include "AEDllDefines.h"

namespace Aetherspark
{
	namespace ImageProcessing
	{
		class AEImageProcessingPipeline;
	
		class aerecognition_EXPORT AEImagePipelineFilter
		{
			public:
				virtual void processImage(IplImage *origImg, IplImage *newImg, AEImageProcessingPipeline *pipeline) = 0;
		};
		
		typedef boost::shared_ptr<AEImagePipelineFilter> AEImagePipelineFilterRef;
	}
}

