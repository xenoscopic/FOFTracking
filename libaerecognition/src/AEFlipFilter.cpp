#include "AEFlipFilter.h"

using namespace Aetherspark::ImageProcessing;

AEFlipFilter::AEFlipFilter(AECVFlipMode mode)
{
	_doFlip = true;
	if((mode & AECVFlipModeHorizontal)
		&& !(mode & AECVFlipModeVertical))
	{
		//Flip horizontally (around y-axis)
		_flipMode = 1;
	}
	else if((mode & AECVFlipModeVertical)
		&& !(mode & AECVFlipModeHorizontal))
	{
		//Flip vertically (around x-axis)
		_flipMode = 0;
	}
	else if((mode & AECVFlipModeHorizontal)
		&& (mode & AECVFlipModeVertical))
	{
		//Flip both
		_flipMode = -1;
	}
	else
	{
		_doFlip = false;
		_flipMode = 0;
	}
	
}

void AEFlipFilter::processImage(IplImage *origImg, IplImage *newImg, AEImageProcessingPipeline *pipeline)
{
	if(_doFlip)
	{
		cvFlip(newImg, NULL, _flipMode);
	}
}

