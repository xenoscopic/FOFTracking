#include "AEMainWindow.h"
#include "AEImageProcessingPipeline.h"
#include "AEFlipFilter.h"
#include "AEHaarFilter.h"

using namespace Aetherspark::Desktop;
using namespace Aetherspark::ImageProcessing;

AEMainWindow::AEMainWindow(QWidget *parent) : QMainWindow(parent)
{
	#ifdef USE_OPENGL_VIDEO

	_video = new AEGLVideoWidget();
	AEImageProcessingPipeline *pipeline = new AEImageProcessingPipeline();
	pipeline->addFilter(AEImagePipelineFilterRef(new AEHaarFilter("/Users/jacob/faces.xml")));
	pipeline->addFilter(AEImagePipelineFilterRef(new AEFlipFilter(AECVFlipModeHorizontal)));
	_video->setPipeline(pipeline);
	
	#else

	_video = new AEVideoWidget();

	#endif
	setCentralWidget(_video);
	resize(500, 400);
}

