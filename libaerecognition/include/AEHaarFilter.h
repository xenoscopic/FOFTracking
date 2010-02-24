#pragma once

#include <string>
#include <stdexcept>
#include "AEImagePipelineFilter.h"

namespace Aetherspark
{
	namespace ImageProcessing
	{	
		class AEHaarFilter : public AEImagePipelineFilter
		{
			public:
				AEHaarFilter(std::string cascadePath) throw(std::invalid_argument);
				~AEHaarFilter();
		
				void processImage(IplImage *origImg, IplImage *newImg, AEImageProcessingPipeline *pipeline);
		
			private:
				CvMemStorage *_storage;
				CvHaarClassifierCascade *_cascade;
		};
	}
}
