#pragma once

#include <string>
#include <stdexcept>
#include "AEImagePipelineFilter.h"

#include "AEDllDefines.h"

namespace Aetherspark
{
	namespace ImageProcessing
	{	
		class aerecognition_EXPORT AEHaarFilter : public AEImagePipelineFilter
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
