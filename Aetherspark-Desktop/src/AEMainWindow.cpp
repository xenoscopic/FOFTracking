#include "AEMainWindow.h"
#include "AEImageProcessingPipeline.h"
#include "AEFlipFilter.h"
#include "AEHaarFilter.h"
#include "AETrackingFilter.h"

#include <string>

#ifdef MACOSX
#include <CoreFoundation/CoreFoundation.h>
#endif

using namespace Aetherspark::Desktop;
using namespace Aetherspark::ImageProcessing;
using namespace std;

AEMainWindow::AEMainWindow(QWidget *parent) : QMainWindow(parent)
{
	//Create video widget
	#ifdef USE_OPENGL_VIDEO
	_video = new AEGLVideoWidget();
	#else
	_video = new AEVideoWidget();
	#endif

	//Setup image pipeline
	AEImageProcessingPipeline *pipeline = new AEImageProcessingPipeline();


	//Locate tracking cascade
	#ifdef WINDOWS
	string cascadePath = "C:\\faces.xml";
	#endif
	
	#ifdef LINUX
	string cascadePath = "/home/jacob/faces.xml";
	#endif
	
	#ifdef MACOSX
	CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
	CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef,
													kCFURLPOSIXPathStyle);
	const char *pathPtr = CFStringGetCStringPtr(macPath,
												CFStringGetSystemEncoding());
	string cascadePath = string(pathPtr) + "/Contents/Resources/Cascades/faces.xml";
	CFRelease(appUrlRef);
	CFRelease(macPath);
	#endif
	
	//Add our filters.  First a cascade tracker, second a flip to make the video work like
	//a mirror.
	pipeline->addFilter(AEImagePipelineFilterRef(new AETrackingFilter(cascadePath.c_str())));
	pipeline->addFilter(AEImagePipelineFilterRef(new AEFlipFilter(AECVFlipModeHorizontal)));
	_video->setPipeline(pipeline);

	setCentralWidget(_video);
	resize(500, 400);
}

