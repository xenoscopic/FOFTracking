#pragma once

#include "AEImagePipelineFilter.h"
#include "AEDllDefines.h"

namespace Aetherspark
{
	namespace ImageProcessing
	{
		enum
		{
			AECVFlipModeHorizontal = 0x1,
			AECVFlipModeVertical = 0x2
		};
		
		typedef unsigned AECVFlipMode;
	
		class aerecognition_EXPORT AEFlipFilter : public AEImagePipelineFilter
		{
			public:
				AEFlipFilter(AECVFlipMode mode);
		
				void processImage(IplImage *origImg, IplImage *newImg, AEImageProcessingPipeline *pipeline);
		
			private:
				int _flipMode;
				bool _doFlip;
		};
	}
}
