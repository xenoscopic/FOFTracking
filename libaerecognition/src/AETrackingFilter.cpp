#include "AETrackingFilter.h"

using namespace std;
using namespace Aetherspark::ImageProcessing;

AETrackingObject::AETrackingObject(IplImage *origImg)
{
	
}

AETrackingObject::~AETrackingObject()
{
	
}

void AETrackingObject::calculateMovement(IplImage *prevImg, IplImage *currImg)
{
	
}

AETrackingFilter::AETrackingFilter(string cascadePath, unsigned identFrameSkip) throw(invalid_argument) :
_frameIndex(0),
_identFrameMod(identFrameSkip + 1)
{
	//Load the classifier cascade
	_cascade = (CvHaarClassifierCascade*)cvLoad(cascadePath.c_str(), 
											   0, 
											   0, 
											   0);
	if(_cascade == NULL)
	{
		throw out_of_range("Specified cascade file is invalid.");
	}

	//Create storage
	_storage = cvCreateMemStorage(0);
}

AETrackingFilter::~AETrackingFilter()
{
	if(_storage != NULL)
	{
		cvReleaseMemStorage(&_storage);
	}

	//No need to check if cascade is null
	//TODO: Check that I am calling this function correctly
	cvRelease((void**)&_cascade);
}

void AETrackingFilter::processImage(IplImage *origImg, IplImage *newImg, AEImageProcessingPipeline *pipeline)
{
	_frameIndex = (_frameIndex + 1) % _identFrameMod;
	if(_frameIndex == 0)
	{
		//Check for any new objects that may have entered the frame
		identifyTrackingCandidates(origImg);
		filterAndInitializeCandidates(origImg);
	}
	
	
}

void AETrackingFilter::identifyTrackingCandidates(IplImage *origImg)
{
	//Clear the memory storage
	cvClearMemStorage(_storage);

	//Find objects
	CvSeq *objects = cvHaarDetectObjects(origImg, 
										 _cascade, 
										 _storage, 
										 1.5, 
										 2, 
										 0,
										 cvSize(30, 30));
	//Loop objects
	int i;
	for(i = 0; i < ((objects != NULL) ? objects->total : 0); i++)
	{
		//Record the rectangle
		_candidates.push_back(*((CvRect*)cvGetSeqElem(objects, i)));
	}
}

void AETrackingFilter::filterAndInitializeCandidates(IplImage *origImg)
{
	list<CvRect>::iterator it;
	
	//Loop over candidates
	for(it = _candidates.begin(); it != _candidates.end(); it++)
	{
		//Focus on the region of interest
		cvSetImageROI(origImg, *it);
		
		//For now just assume the match is good
		_objects.push_back(AETrackingObjectRef(new AETrackingObject(origImg)));
		
		//Reset the ROI
		cvResetImageROI(origImg);
	}
	
	_candidates.clear();
}
