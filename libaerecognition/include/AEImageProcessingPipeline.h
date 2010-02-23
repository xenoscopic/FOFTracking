#pragma once

#include <vector>
#include <stdexcept>
#include "AEImagePipelineFilter.h"

namespace Aetherspark
{
	namespace ImageProcessing
	{
		class AEImagePipelineFilter;
	
		class AEImageProcessingPipeline
		{
			public:
				AEImageProcessingPipeline();
			
				void AddFilter(AEImagePipelineFilterRef filter);
				void InsertFilter(AEImagePipelineFilterRef filter, unsigned long filterIndex) throw(std::out_of_range);
				void RemoveFilter(unsigned long filterIndex) throw(std::out_of_range);
				void RemoveAllFilters();
				void ToggleFilters(bool on);
				
				IplImage* processImage(IplImage *img);
		
			private:
				bool _filtersOn;
				std::vector<AEImagePipelineFilterRef> _filters;
		};
	}
}

