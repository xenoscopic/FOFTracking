#include "AEMainWindow.h"
#include "AEImageProcessingPipeline.h"
#include "AEFlipFilter.h"
#include "AEHaarFilter.h"
#include "AETrackingFilter.h"

using namespace Aetherspark::Desktop;
using namespace Aetherspark::ImageProcessing;

AEMainWindow::AEMainWindow(QWidget *parent) : QMainWindow(parent)
{
	#ifdef USE_OPENGL_VIDEO
	_video = new AEGLVideoWidget();
	#else
	_video = new AEVideoWidget();
	#endif

	AEImageProcessingPipeline *pipeline = new AEImageProcessingPipeline();

	#ifdef WINDOWS
	pipeline->addFilter(AEImagePipelineFilterRef(new AETrackingFilter("C:\\faces.xml")));
	#endif
	#ifdef LINUX
	pipeline->addFilter(AEImagePipelineFilterRef(new AETrackingFilter("/home/jacob/faces.xml")));
	#endif
	#ifdef MACOSX
	pipeline->addFilter(AEImagePipelineFilterRef(new AETrackingFilter("/Users/jacob/faces.xml")));
	#endif

	pipeline->addFilter(AEImagePipelineFilterRef(new AEFlipFilter(AECVFlipModeHorizontal)));
	_video->setPipeline(pipeline);

	setCentralWidget(_video);
	resize(500, 400);
}

