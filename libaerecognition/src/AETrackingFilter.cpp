#include "AETrackingFilter.h"

using namespace std;
using namespace Aetherspark::ImageProcessing;

#define MIN_DIST 10

AETrackingObject::AETrackingObject(IplImage *grey, CvRect roi)
{
	//Set the region of interest and work within that for identifying
	//our tracking features
	
}

AETrackingObject::~AETrackingObject()
{
	
}

void AETrackingObject::calculateMovement(IplImage *grey, IplImage *prevGrey, IplImage *pyramid, IplImage *prevPyramid, int flags)
{
	
}

AETrackingFilter::AETrackingFilter(string cascadePath, unsigned identFrameSkip) throw(invalid_argument) :
_frameIndex(0),
_identFrameMod(identFrameSkip + 1),
_initialized(false)
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
	
	if(_initialized)
	{
		cvReleaseImage(&_grey);
		cvReleaseImage(&_prevGrey);
		cvReleaseImage(&_pyramid);
		cvReleaseImage(&_prevPyramid);
	}
}

void AETrackingFilter::processImage(IplImage *origImg, IplImage *newImg, AEImageProcessingPipeline *pipeline)
{
	if(!_initialized)
	{
		//We can't initialize until we get the frame because we don't know it's
		//size
		_grey = cvCreateImage(cvGetSize(origImg), IPL_DEPTH_8U, 1);
		_prevGrey = cvCreateImage(cvGetSize(origImg), IPL_DEPTH_8U, 1);
		_pyramid = cvCreateImage(cvGetSize(origImg), IPL_DEPTH_8U, 1);
		_prevPyramid = cvCreateImage(cvGetSize(origImg), IPL_DEPTH_8U, 1);
		_pyramidFlags = 0; //_prevPyramid will not initially be calculated
		
		//_prevGrey will be empty, and if there is a Haar match, we need
		//to run optical flow calculations, therefore we should just make
		//it a copy of _grey for the first semi-bogus calculation.
		cvCvtColor(origImg, _grey, CV_BGR2GRAY);
		cvCopy(_grey, _prevGrey, 0);
		
		_initialized = true;
	}
	
	//Create a grey-scale image of the frame
	cvCvtColor(origImg, _grey, CV_BGR2GRAY);
	
	//If this is the Nth frame
	_frameIndex = (_frameIndex + 1) % _identFrameMod;
	if(_frameIndex == 0)
	{
		//Check for any new objects that may have entered the frame
		identifyTrackingCandidates(origImg);
		filterAndInitializeCandidates(_grey);
	}
	
	//Now update any objects we're tracking
	list<AETrackingObjectRef>::iterator it;
	_pyramidFlags &= ~CV_LKFLOW_PYR_B_READY; //_pyramid will not be ready before the first run
	for(it = _objects.begin(); it != _objects.end(); it++)
	{
		(*it)->calculateMovement(_grey, _prevGrey, _pyramid, _prevPyramid, _pyramidFlags);
		_pyramidFlags |= CV_LKFLOW_PYR_B_READY; //Current frame pyramid already calculated, no need to redo
		//TODO: Add object removal
	}
	
	//Determine if the previous frame pyramid has been calculated
	if(_objects.size() > 0)
	{
		//We did a calculation, so the previous frame pyramid will be good to go
		_pyramidFlags |= CV_LKFLOW_PYR_A_READY;
	}
	else
	{
		//We didn't do a calculation, so the previous pyramid is not valid
		_pyramidFlags = 0;
	}
	
	//Swap out images
	CV_SWAP(_prevGrey, _grey, _swapBuffer);
	CV_SWAP(_prevPyramid, _pyramid, _swapBuffer);
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

void AETrackingFilter::filterAndInitializeCandidates(IplImage *grey)
{
	list<CvRect>::iterator it;
	
	//Loop over candidates
	for(it = _candidates.begin(); it != _candidates.end(); it++)
	{
		//For now just assume the match is good
		_objects.push_back(AETrackingObjectRef(new AETrackingObject(grey, *it)));
	}
	
	_candidates.clear();
}
