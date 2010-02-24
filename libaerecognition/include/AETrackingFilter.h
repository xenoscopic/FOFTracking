#pragma once

#include <string>
#include <stdexcept>
#include "AEImagePipelineFilter.h"

namespace Aetherspark
{
	namespace ImageProcessing
	{	
		class AETrackingFilter : public AEImagePipelineFilter
		{
			public:
				AETrackingFilter(std::string cascadePath) throw(std::invalid_argument);
				~AETrackingFilter();
		
				void processImage(IplImage *origImg, IplImage *newImg, AEImageProcessingPipeline *pipeline);
		
			private:
				CvMemStorage *_storage;
				CvHaarClassifierCascade *_cascade;
		};
	}
}
