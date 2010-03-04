#pragma once

#include <string>
#include <stdexcept>
#include <list>

#include <boost/shared_ptr.hpp>

#include "AEDllDefines.h"
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
				AETrackingObject(IplImage *orig, IplImage *grey, CvRect roi);
				~AETrackingObject();
				
				CvPoint2D32f center();
				CvRect boundingBox();
				bool lost();
				
			private:
				//Fills _points[0] up with N_FEATURE_TRACK good features.
				//It uses the Kolsch-Turk criteria to select features.
				void fillGoodFeatures(IplImage *orig, IplImage *grey, CvRect roi, bool initial);
				
				//Recalculates center using LK optical-flow algorithm.
				void calculateMovement(IplImage *orig, IplImage *grey, IplImage *prevGrey, IplImage *pyramid, IplImage *prevPyramid, int flags);
				
				//_points[0] will be the coordinates in prevGrey
				//_points[1] will be the coordinates in grey (which will be calculated)
				CvPoint2D32f* _points[2];
				CvPoint2D32f *_swapBuffer;
				CvHistogram *_hist;
				CvPoint2D32f _center;
				CvRect _boundingBox;
				char *_status;
				int _count;
				bool _lost;
		};
		
		typedef boost::shared_ptr<AETrackingObject> AETrackingObjectRef;
		
		class aerecognition_EXPORT AETrackingFilter : public AEImagePipelineFilter
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
				void filterAndInitializeCandidates(IplImage *origImg, IplImage *grey);
				
				//Index trackers
				unsigned _frameIndex;
				unsigned _identFrameMod;
				
				//Haar matching storage
				CvMemStorage *_storage;
				CvHaarClassifierCascade *_cascade;
				
				//Tracking buffers
				bool _initialized;
				IplImage *_grey;
				IplImage *_prevGrey;
				IplImage *_pyramid;
				IplImage *_prevPyramid;
				IplImage *_swapBuffer; //Never initialized, just used for CV_SWAP
				int _pyramidFlags; //Tell cvCalcOpticalFlowPyrLK which pyramids are already calculated
				
				//Tracking objects
				std::list<CvRect> _candidates;
				std::list<AETrackingObjectRef> _objects;
		};
	}
}
