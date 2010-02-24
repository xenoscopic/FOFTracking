#pragma once

#include <string>
#include <stdexcept>
#include <list>

#include <boost/shared_ptr.hpp>

#include "AEImagePipelineFilter.h"

namespace Aetherspark
{
	namespace ImageProcessing
	{
		class AETrackingFilter;
		
		class AETrackingObject
		{
			friend class AETrackingFilter;
			
			public:
				//ROI will already be set when this is called
				AETrackingObject(IplImage *origImg);
				~AETrackingObject();
				
			private:
				void calculateMovement(IplImage *prevImg, IplImage *currImg);
				
				
		};
		
		typedef boost::shared_ptr<AETrackingObject> AETrackingObjectRef;
		
		class AETrackingFilter : public AEImagePipelineFilter
		{
			public:
				//identFrameSkip is how many frames should be skipped between each identification
				//stage.
				AETrackingFilter(std::string cascadePath, unsigned identFrameSkip = 10) throw(std::invalid_argument);
				~AETrackingFilter();
		
				void processImage(IplImage *origImg, IplImage *newImg, AEImageProcessingPipeline *pipeline);
		
			private:
				//Looks for Haar-Cascade matches and adds them to a list of candidates
				void identifyTrackingCandidates(IplImage *origImg);
				//Looks at the candidate lists and filters it down, then creates a tracking
				//feature set for all remaining objects.
				void filterAndInitializeCandidates(IplImage *origImg);
				
				unsigned _frameIndex;
				unsigned _identFrameMod;
				
				CvMemStorage *_storage;
				CvHaarClassifierCascade *_cascade;
				
				std::list<CvRect> _candidates;
				std::list<AETrackingObjectRef> _objects;
		};
	}
}
