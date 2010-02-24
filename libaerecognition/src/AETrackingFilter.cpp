#include "AETrackingFilter.h"

using namespace std;
using namespace Aetherspark::ImageProcessing;

AETrackingFilter::AETrackingFilter(string cascadePath) throw(invalid_argument)
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
	//Boilerplate variables
	CvPoint pt1, pt2;
	int i;

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
	for(i = 0; i < ((objects != NULL) ? objects->total : 0); i++)
	{
		//Create a new rectangle for drawing the object
		CvRect *r = (CvRect*)cvGetSeqElem(objects, i);

		//Find the dimensions of the face
		pt1.x = r->x;
		pt2.x = r->x + r->width;
		pt1.y = r->y;
		pt2.y = r->y + r->height;

		//Draw the rectangle
		cvRectangle(newImg, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0);
	}
}

