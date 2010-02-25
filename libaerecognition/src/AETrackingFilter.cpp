#include "AETrackingFilter.h"

#include <cstdio>
#include <cmath>

using namespace std;
using namespace Aetherspark::ImageProcessing;

//Number of features to track
#define N_FEATURE_TRACK 35
//Minimum distance between features
#define MIN_FEAT_DIST 10
//Minimum distance between objects (to avoid dupes)
#define MIN_OBJECT_DIST 200
//As defined in the Kolsch-Turk paper (flocks of features)
#define K_FOF 3
#define FEATURE_QUALITY 0.01
#define WIN_SIZE 10
//Minimum area for a Haar match
#define MIN_AREA 20000

AETrackingObject::AETrackingObject(IplImage *orig, IplImage *grey, CvRect roi) :
_boundingBox(roi),
_count(0),
_lost(false)
{	
	printf("New tracking object\n");
	fflush(stdout);
	
	//Allocate our point buffers
	_points[0] = (CvPoint2D32f*)cvAlloc(N_FEATURE_TRACK*K_FOF*sizeof(CvPoint2D32f));
	_points[1] = (CvPoint2D32f*)cvAlloc(N_FEATURE_TRACK*K_FOF*sizeof(CvPoint2D32f));
	_status = (char*)cvAlloc(N_FEATURE_TRACK*K_FOF*sizeof(char));
	
	fillGoodFeatures(orig, grey, roi);
}

AETrackingObject::~AETrackingObject()
{
	//Deallocate point buffers
	cvFree((void**)(&(_points[0])));
	cvFree((void**)(&(_points[1])));
	cvFree((void**)(&_status));
}

CvPoint2D32f AETrackingObject::center()
{
	return _center;
}

bool AETrackingObject::lost()
{
	return _lost;
}

void AETrackingObject::fillGoodFeatures(IplImage *orig, IplImage *grey, CvRect roi)
{
	//Set the region of interest and work within that for identifying
	//our tracking features
	cvSetImageROI(grey, roi);
	
	//Boilerplate variables (cvGetSize respects ROI)
	//Make a small copy of the grey ROI because cvGoodFeaturesToTrack doesn't support ROI
	IplImage *greyCopy = cvCreateImage(cvGetSize(grey), grey->depth, grey->nChannels);
	cvCopy(grey, greyCopy, NULL);
	IplImage *eig = cvCreateImage(cvGetSize(greyCopy), IPL_DEPTH_32F, 1);
	IplImage *temp = cvCreateImage(cvGetSize(greyCopy), IPL_DEPTH_32F, 1);
	
	//Figure out how many features we need to identify
	int tempCount = (N_FEATURE_TRACK - _count)*K_FOF;
	
	//Find good features to track
	cvGoodFeaturesToTrack(greyCopy, eig, temp, _points[0], &tempCount, FEATURE_QUALITY,
		MIN_FEAT_DIST, NULL, 3, 0, 0.04);
	cvFindCornerSubPix(greyCopy, _points[0], tempCount, cvSize(WIN_SIZE, WIN_SIZE), cvSize(-1, -1),
		cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 20, 0.03));
	
	printf("Found %i good features (%i, %i)\n", tempCount, roi.x, roi.y);
	fflush(stdout);
	
	//Loop over the identified points and adjust them to ignore the ROI offset
	for(int i = 0; i < tempCount; i++)
	{
		_points[0][i].x += roi.x;
		_points[0][i].y += roi.y;
	}
	fflush(stdout);
	
	//TODO: Use color histogram to trim number of points
	
	
	//Record the actual count
	_count = tempCount;
	
	//Free up memory
	cvReleaseImage(&greyCopy);
	cvReleaseImage(&eig);
	cvReleaseImage(&temp);
	
	//Reset region of interest
	cvResetImageROI(grey);
}

void AETrackingObject::calculateMovement(IplImage *orig, IplImage *grey, IplImage *prevGrey, IplImage *pyramid, IplImage *prevPyramid, int flags)
{
	//Calculate the optical flow
	cvCalcOpticalFlowPyrLK(prevGrey, grey, prevPyramid, pyramid, _points[0], _points[1], _count, cvSize(WIN_SIZE, WIN_SIZE),
		3, _status, 0, cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 20, 0.03), flags);
		
	//Remove points who didn't make the journey
	int i, k;
	for(i = k = 0; i < _count; i++)
	{
		if(_status[i] == 0)
		{
			//Low correlation, remove point
			continue;
		}
		
		_points[1][k++] = _points[1][i];
	}
	_count = k;
	
	//Calculate the center of the features
	_center.x = 0.0;
	_center.y = 0.0;
	for(i = 0; i < _count; i++)
	{
		_center.x += _points[1][i].x;
		_center.y += _points[1][i].y;
	}
	_center.x /= _count;
	_center.y /= _count;
	
	//Relocate the bounding box to center it on the median feature
	_boundingBox.x = _center.x - (float)_boundingBox.width/2;
	_boundingBox.y = _center.x - (float)_boundingBox.height/2;
	
	//For each feature, check it against the Kolsch-Turk criteria
	
	//Swap the point buffers
	CV_SWAP(_points[0], _points[1], _swapBuffer);
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
		cvCopy(_grey, _prevGrey, NULL);
		
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
		filterAndInitializeCandidates(origImg, _grey);
	}
	
	//Now update any objects we're tracking
	list<AETrackingObjectRef>::iterator it;
	_pyramidFlags &= ~CV_LKFLOW_PYR_B_READY; //_pyramid will not be ready before the first run
	for(it = _objects.begin(); it != _objects.end(); it++)
	{
		(*it)->calculateMovement(origImg, _grey, _prevGrey, _pyramid, _prevPyramid, _pyramidFlags);
		_pyramidFlags |= CV_LKFLOW_PYR_B_READY; //Current frame pyramid already calculated, no need to redo
		
		if((*it)->lost())
		{
			it = _objects.erase(it);
			it--; //This will be pointing at the element that was next, so decrement before incrementing
			continue;
		}
		
		//Draw a green dot
		cvCircle(newImg, cvPointFrom32f((*it)->center()), 3, CV_RGB(0,255,0), -1, 8, 0);
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
	bool goodCandidate;
	for(i = 0; i < ((objects != NULL) ? objects->total : 0); i++)
	{
		_candidates.push_back(*((CvRect*)cvGetSeqElem(objects, i)));
	}
}

CvPoint2D32f rectCenter(CvRect rect)
{
	CvPoint2D32f ret;
	ret.x = rect.x + (float)rect.width/2;
	ret.y = rect.y + (float)rect.height/2;
	return ret;
}

float euclideanDistance(CvPoint2D32f x0, CvPoint2D32f x1)
{
	return sqrt(pow(x1.x - x0.x, 2) + pow(x1.y - x0.y, 2));
}

void AETrackingFilter::filterAndInitializeCandidates(IplImage *origImg, IplImage *grey)
{
	list<CvRect>::iterator it;
	list<AETrackingObjectRef>::iterator oit;
	
	//Loop over candidates
	bool doContinue;
	for(it = _candidates.begin(); it != _candidates.end(); it++)
	{
		//Make sure the new object isn't too close to the old one
		doContinue = false;
		CvPoint2D32f center = rectCenter(*it);
		for(oit = _objects.begin(); oit != _objects.end(); oit++)
		{
			if(euclideanDistance((*oit)->center(), center) < MIN_OBJECT_DIST)
			{
				doContinue = true;
				break;
			}
		}
		if(doContinue)
		{
			continue;
		}
		
		//Make sure the match is big enough
		if((it->width * it->height) < MIN_AREA)
		{
			continue;
		}
		
		//For now just assume the match is good
		_objects.push_back(AETrackingObjectRef(new AETrackingObject(origImg, grey, *it)));
	}
	
	_candidates.clear();
}
