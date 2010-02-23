#pragma once

#include "AEImagePipelineFilter.h"

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
	
		class AEFlipFilter : public AEImagePipelineFilter
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
