#import "AECaptureDevice.h"

using namespace Aetherspark::Capture;

#define INPUT_WIDTH 320.0
#define INPUT_HEIGHT 240.0

@implementation QTCapture

-(id)init
{
	if(self = [super init])
	{
		//Create the OpenCV image we'll use to handle the QT frame.
		//We don't use the OpenCV creation functions because we're grabbing
		//data out of Core Video
		frameHeader = (IplImage*)malloc(sizeof(IplImage));
		
		//Load the QT video input
		captureSession = [[QTCaptureSession alloc] init];
		captureVideoOutput = [[QTCaptureDecompressedVideoOutput alloc] init];
		[captureVideoOutput setPixelBufferAttributes:[NSDictionary dictionaryWithObjectsAndKeys:
													  [NSNumber numberWithDouble:INPUT_WIDTH], (id)kCVPixelBufferWidthKey,
													  [NSNumber numberWithDouble:INPUT_HEIGHT], (id)kCVPixelBufferHeightKey,
													  [NSNumber numberWithUnsignedInt:kCVPixelFormatType_24RGB], (id)kCVPixelBufferPixelFormatTypeKey,
													  nil]];
		[captureVideoOutput setAutomaticallyDropsLateVideoFrames:YES];
		[captureVideoOutput setDelegate:self];

		//Find a video device
		BOOL success;
		NSError* error;
		QTCaptureDevice* videoDevice = [QTCaptureDevice defaultInputDeviceWithMediaType:QTMediaTypeVideo];
		success = [videoDevice open:&error];

		if(error != nil)
		{
			NSLog(@"Couldn't open video capture device: %@.", [error localizedDescription]);
			[self release];
			return nil;
		}

		if(videoDevice)
		{
			//Create an input
			captureDeviceInput = [[QTCaptureDeviceInput alloc] initWithDevice:videoDevice];

			//Set the session input
			success = [captureSession addInput:captureDeviceInput error:&error];
			if(!success)
			{
				NSLog(@"Couldn't set up the input device: %@.", [error localizedDescription]);
				[self release];
				return nil;
			}

			//Set the session output
			success = [captureSession addOutput:captureVideoOutput error:&error];
			if(!success)
			{
				NSLog(@"Couldn't set up the output device: %@.", [error localizedDescription]);
				[self release];
				return nil;
			}

			//Start the session
			[captureSession startRunning];
		}
	}
	return self;
}

-(void)dealloc
{
	//Stop the capture session and close the device
	[captureSession stopRunning];
	if([[captureDeviceInput device] isOpen])
	{
		[[captureDeviceInput device] close];
	}
	//Release resources
	[captureDeviceInput release];
	[captureVideoOutput release];
	[captureSession release];
	free(frameHeader);
	//Don't need to check initialization because this function
	//will be a no-op if frameBuffer is null, which it will be
	//by default.
	cvReleaseImage(&frameBuffer);
	[super dealloc];
}

- (void)captureOutput:(QTCaptureOutput *)captureOutput didOutputVideoFrame:(CVImageBufferRef)videoFrame withSampleBuffer:(QTSampleBuffer *)sampleBuffer fromConnection:(QTCaptureConnection *)connection;
{
	//Lock the image data
	CVPixelBufferLockBaseAddress((CVPixelBufferRef)videoFrame, 0);
	
	//Fill in the OpenCV image data with CoreVideo image data
	frameHeader->nSize = sizeof(IplImage);
	frameHeader->ID = 0;
	frameHeader->nChannels = 3;
	frameHeader->depth = IPL_DEPTH_8U;
	frameHeader->dataOrder = 0;
	frameHeader->origin = 0; //Top left origin
	frameHeader->width = CVPixelBufferGetWidth((CVPixelBufferRef)videoFrame);
	frameHeader->height = CVPixelBufferGetHeight((CVPixelBufferRef)videoFrame);
	frameHeader->roi = 0; //Region of interest
	frameHeader->maskROI = 0;
	frameHeader->imageId = 0;
	frameHeader->tileInfo = 0;
	frameHeader->imageSize = CVPixelBufferGetDataSize((CVPixelBufferRef)videoFrame);
	frameHeader->imageData = (char*)CVPixelBufferGetBaseAddress((CVPixelBufferRef)videoFrame);
	frameHeader->widthStep = CVPixelBufferGetBytesPerRow((CVPixelBufferRef)videoFrame);
	frameHeader->imageDataOrigin = (char*)CVPixelBufferGetBaseAddress((CVPixelBufferRef)videoFrame);
	
	//Copy the data from the QT buffer into the local buffer
	@synchronized(self)
	{
		if(!initialized)
		{
			frameBuffer = cvCreateImage(cvGetSize(frameHeader), IPL_DEPTH_8U, 3);
		}
		cvCopy(frameHeader, frameBuffer);
	}
	
	//Unlock the image data
	CVPixelBufferUnlockBaseAddress((CVPixelBufferRef)videoFrame, 0);
}

-(IplImage*)latestFrame
{
	@synchronized(self)
	{
		return cvCloneImage(frameBuffer);
	}
	return nil; //Here to silence compiler warning about returning in @synchronized.
}


@end

AECaptureDevice::AECaptureDevice()
{
	_capture = [[QTCapture alloc] init];
}

AECaptureDevice::~AECaptureDevice()
{
	[_capture release];
}

IplImage* AECaptureDevice::captureFrame()
{
	return [_capture latestFrame];
}


