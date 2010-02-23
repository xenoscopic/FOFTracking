#pragma once

#include <opencv/cv.h>
#include <boost/shared_ptr.hpp>

namespace Aetherspark
{
	namespace ImageProcessing
	{
		class AEImageProcessingPipeline;
	
		class AEImagePipelineFilter
		{
			public:
				virtual void processImage(IplImage *origImg, IplImage *newImg, AEImageProcessingPipeline *pipeline) = 0;
		};
		
		typedef boost::shared_ptr<AEImagePipelineFilter> AEImagePipelineFilterRef;
	}
}

