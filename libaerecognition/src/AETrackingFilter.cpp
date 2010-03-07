#include "AETrackingFilter.h"

#include <cstdio>
#include <cmath>
#include <algorithm>
#include <cstring> //For memset

#include "AEDualSort.h"

using namespace std;
using namespace Aetherspark::ImageProcessing;
using namespace Aetherspark::Utility;

//Number of features to track
#define N_FEATURE_TRACK 55
//Minimum distance between features
#define MIN_FEAT_DIST 10
//Minimum distance between objects (to avoid dupes)
#define MIN_OBJECT_DIST 200
//As defined in the Kolsch-Turk paper (flocks of features)
#define K_FOF 3
#define FEATURE_QUALITY 0.001
#define WIN_SIZE 10
//Minimum area for a Haar match
#define MIN_AREA 20000

//Convenience methods/objects
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

bool pointIsInRect(CvPoint2D32f point, CvRect rect)
{
	return ((point.x >= rect.x) && (point.x <= (rect.x + rect.width))
		&& (point.y >= rect.y) && (point.y <= (rect.y + rect.height)));
}

class HistogramSorter
{
	public:
		HistogramSorter(IplImage *backProjection) :
		_backProjection(backProjection),
		_iSize(cvGetSize(backProjection)) { }
		
		bool operator() (CvPoint2D32f i, CvPoint2D32f j)
		{
			//Convert to useful coordinates
			CvPoint iPt = cleanPoint(cvPointFrom32f(i));
			CvPoint jPt = cleanPoint(cvPointFrom32f(j));
			
			//Get the hue value for each channel from the image
			int iHue = (int)cvGet2D(_backProjection, iPt.x, iPt.y).val[0];
			int jHue = (int)cvGet2D(_backProjection, jPt.x, jPt.y).val[0];
			
			//Return this so that the points will be sorted in
			//descending order
			return iHue > jHue;
		}
		
	private:
		IplImage *_backProjection;
		CvSize _iSize;
		
		CvPoint cleanPoint(CvPoint point)
		{
			//Check to make sure the conversion hasn't put them outside
			//the image boundaries
			CvPoint ret;
			ret.x = max(0, point.x);
			ret.x = min(_iSize.width - 1, ret.x);
			ret.y = max(0, point.y);
			ret.y = min(_iSize.height - 1, ret.y);
			return ret;
		}
};

//Actual class implementations

AETrackingObject::AETrackingObject(IplImage *orig, IplImage *grey, CvRect roi) :
_hist(NULL),
_boundingBox(roi),
_count(0),
_lost(false)
{	
	printf("New tracking object\n");
	fflush(stdout);
	
	//Allocate our point buffers
	_points[0] = new CvPoint2D32f[N_FEATURE_TRACK*K_FOF];
	_points[1] = new CvPoint2D32f[N_FEATURE_TRACK*K_FOF];
	_status = new char[N_FEATURE_TRACK*K_FOF];
	_frameCounts = new unsigned[N_FEATURE_TRACK];
	
	//Create an empty hue histogram
	int numBins = 256;
	float hueRanges[] = {0, 181}; //Cover 180 degrees of hue
	float *ranges[] = {hueRanges};
	_hist = cvCreateHist(1, &numBins, CV_HIST_ARRAY, ranges, 1);
	
	fillGoodFeatures(orig, grey, roi, true);
}

AETrackingObject::~AETrackingObject()
{
	//Deallocate point buffers
	delete [] _points[0];
	delete [] _points[1];
	delete [] _status;
	delete [] _frameCounts;

	//Release histogram
	cvReleaseHist(&_hist);
}

CvPoint2D32f AETrackingObject::center()
{
	return _center;
}

CvRect AETrackingObject::boundingBox()
{
	return _boundingBox;
}

bool AETrackingObject::lost()
{
	return _lost;
}

void AETrackingObject::fillGoodFeatures(IplImage *orig, IplImage *grey, CvRect roi, bool initial)
{
	//Set the region of interest and work within that for identifying
	//our tracking features
	cvSetImageROI(grey, roi);
	cvSetImageROI(orig, roi);
	
	//Boilerplate variables (cvGetSize respects ROI)
	//Make a small copy of the grey ROI because cvGoodFeaturesToTrack doesn't support ROI
	IplImage *greyCopy = cvCreateImage(cvGetSize(grey), grey->depth, grey->nChannels);
	cvCopy(grey, greyCopy, NULL);
	IplImage *eig = cvCreateImage(cvGetSize(greyCopy), IPL_DEPTH_32F, 1);
	IplImage *temp = cvCreateImage(cvGetSize(greyCopy), IPL_DEPTH_32F, 1);
	IplImage *hsvImage = cvCreateImage(cvGetSize(orig), IPL_DEPTH_8U, 3);
	IplImage *hueImage = cvCreateImage(cvGetSize(orig), IPL_DEPTH_8U, 1);
	cvCvtColor(orig, hsvImage, CV_BGR2HSV);
	cvSetImageCOI(hsvImage, 1); //Set COI to hue channel
	cvCopy(hsvImage, hueImage);
	cvSetImageCOI(hsvImage, 0); //Reset COI, probably not necessary
	IplImage *backProjection = cvCreateImage(cvGetSize(hueImage), IPL_DEPTH_8U, 1);
	cvCalcBackProject(&hueImage, backProjection, _hist);
	
	//Find good features to track
	int tempCount = N_FEATURE_TRACK*K_FOF;
	cvGoodFeaturesToTrack(greyCopy, eig, temp, _points[0], &tempCount, FEATURE_QUALITY,
		MIN_FEAT_DIST, NULL, 3, 0, 0.04);
	cvFindCornerSubPix(greyCopy, _points[0], tempCount, cvSize(WIN_SIZE, WIN_SIZE), cvSize(-1, -1),
		cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 20, 0.03));

	if(initial)
	{
		printf("Found %i good features (%i, %i)\n", tempCount, roi.x, roi.y);
		fflush(stdout);
		
		//Create hue histogram from initial image
		cvCalcHist(&hueImage, _hist);
	}
	
	//Use color histogram to sort point rank.
	//Do this BEFORE adjusting for ROI, since hueImage
	//will just be of ROI.  Don't need to worry about 
	//resorting _frameCounts here because these will
	//all just be assigned a framecount of 0.
	HistogramSorter hSorter(backProjection);
	sort(_points[0], _points[0] + tempCount, hSorter);
	
	//Loop over the identified points and adjust them to ignore the ROI offset
	int i;
	for(i = 0; i < tempCount; i++)
	{
		_points[0][i].x += roi.x;
		_points[0][i].y += roi.y;
	}
	
	//Fill in as many points as we need
	if(!initial)
	{
		//This method is being called from calculateMovement, so
		//we need to transfer the points from _points[0] to _points[1]
		i = 0;
		while((i < tempCount) && (_count < N_FEATURE_TRACK))
		{
			//Make sure this point is far enough away from existing ones.
			//This doesn't really take quality into account, but in order
			//to do that, we'd have to sort after every add.
			int p;
			for(p = 0; p < _count; p++)
			{
				if(euclideanDistance(_points[0][i], _points[1][p]) < MIN_FEAT_DIST)
				{
					break;
				}
			}
			if(p != _count)
			{
				i++;
				continue;
			}
			//Looks good, add the point.
			_points[1][_count] = _points[0][i++];
			_frameCounts[_count] = 0;
			_count++;
		}
		//Resort based on quality so that future removes
		//will take it into account.  We use dual sort
		//because we also need to sort _frameCounts.
		dual_sort<CvPoint2D32f*, unsigned*, CvPoint2D32f, unsigned, HistogramSorter>
			(_points[1], _points[1] + _count, _frameCounts, _frameCounts + _count, hSorter);
		// sort(_points[1], _points[1] + _count, hSorter);
	}
	else
	{
		_count = min(N_FEATURE_TRACK, tempCount);
		//Make all framecounts 0
		memset((void*)_frameCounts, 0, N_FEATURE_TRACK*sizeof(unsigned));
	}
	
	//Free up memory
	cvReleaseImage(&greyCopy);
	cvReleaseImage(&eig);
	cvReleaseImage(&temp);
	cvReleaseImage(&hsvImage);
	cvReleaseImage(&hueImage);
	cvReleaseImage(&backProjection);
	
	//Reset region of interest
	cvResetImageROI(grey);
	cvResetImageROI(orig);
}

void AETrackingObject::calculateMovement(IplImage *orig, IplImage *grey, IplImage *prevGrey, IplImage *pyramid, IplImage *prevPyramid, int flags)
{
	if(_count == 0)
	{
		//We have no features to track on!
		_lost = true;
		return;
	}
	
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
		
		_points[1][k] = _points[1][i];
		_frameCounts[k] = _frameCounts[i] + 1;
		k++;
	}
	_count = k;
	
	//Calculate the center of the features, weighted based on
	//how long points have been around for.
	double frameSum = 0.0;
	for(i = 0; i < _count; i++)
	{
		frameSum += _frameCounts[i];
	}
	_center.x = 0.0;
	_center.y = 0.0;
	double ptWeight;
	for(i = 0; i < _count; i++)
	{
		ptWeight = (double)_frameCounts[i]/frameSum;
		_center.x += _points[1][i].x*ptWeight;
		_center.y += _points[1][i].y*ptWeight;
	}
	
	//Relocate the bounding box to center it on the median feature
	_boundingBox.x = _center.x - (float)_boundingBox.width/2;
	_boundingBox.y = _center.y - (float)_boundingBox.height/2;
	
	//If the box is now outside the image frame, mark the object as lost
	CvSize iSize = cvGetSize(orig);
	if((_boundingBox.x < 0)
		|| (_boundingBox.y < 0)
		|| ((_boundingBox.x + _boundingBox.width) > iSize.width)
		|| ((_boundingBox.y + _boundingBox.height) > iSize.height))
	{
		_lost = true;
		return;
	}

	//For each feature, check it against the Kolsch-Turk criteria
	int p;
	for(i = k = 0; i < _count; i++)
	{
		if(!pointIsInRect(_points[1][i], _boundingBox))
		{
			//Point has moved outside bounding box
			continue;
		}
		
		//Make sure this point is minDistance from all the ones
		//before it.  If it is too close to the ones before it,
		//then remove it because it will have a lower back-projection
		//ranking than the point before it.
		for(p = 0; p < k; p++)
		{
			if(euclideanDistance(_points[1][p], _points[1][i]) < MIN_FEAT_DIST)
			{
				break;
			}
		}
		if(p != k)
		{
			//We found a point that was too close
			continue;
		}
		
		_points[1][k++] = _points[1][i];
	}
	_count = k;
	
	//Regenerate the missing points
	fillGoodFeatures(orig, grey, _boundingBox, false);
	
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
		throw invalid_argument("Specified cascade file is invalid.");
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
			if(_objects.size() == 0)
			{
				break;
			}
			//This will be pointing at the element that was next, so decrement before incrementing.
			it--;
			continue;
		}
		
		//Draw a green dot
		cvCircle(newImg, cvPointFrom32f((*it)->center()), 3, CV_RGB(0,255,0), -1, 8, 0);
		
		//Draw features
		for(int i = 0; i < (*it)->_count; i++)
		{
			cvCircle(newImg, cvPointFrom32f((*it)->_points[0][i]), 3, CV_RGB(255,0,0), -1, 8, 0);
		}
		
		//Draw bounding box
		CvPoint x0, x1;
		CvRect bBox = (*it)->boundingBox();
		x0.x = bBox.x;
		x0.y = bBox.y;
		x1.x = bBox.x + bBox.width;
		x1.y = bBox.y + bBox.height;
		cvRectangle(newImg, x0, x1, CV_RGB(255, 0, 255), 1, 8, 0);
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
		_candidates.push_back(*((CvRect*)cvGetSeqElem(objects, i)));
	}
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
			if(pointIsInRect(center, (*oit)->boundingBox()))
			{
				doContinue = true;
				break;
			}
			else
			{
				CvRect bBox = (*oit)->boundingBox();
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
