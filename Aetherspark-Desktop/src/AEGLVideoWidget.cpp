#include "AEGLVideoWidget.h"

using namespace Aetherspark::Desktop;
using namespace Aetherspark::Capture;
using namespace Aetherspark::ImageProcessing;

//Apparently this method has been removed or renamed or something
void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	GLdouble xmin, xmax, ymin, ymax;
	
	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;
	
	glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

AEGLVideoWidget::AEGLVideoWidget(QWidget *parent) : QGLWidget(parent),
_pipeline(NULL)
{
	//Set up the capture
	_capture = new AECaptureDevice(); //TODO: Add camera selection
	
	//Set up timer (parent will destroy)
	_updateTimer = new QTimer(this);
	connect(_updateTimer, SIGNAL(timeout()), this, SLOT(updateImage()));
	_updateTimer->start(50);
}

AEGLVideoWidget::~AEGLVideoWidget()
{
	//Get rid of the capture
	delete _capture;
	
	//Destroy the pipeline
	if(_pipeline != NULL)
	{
		delete _pipeline;
	}
}

AEImageProcessingPipeline* AEGLVideoWidget::pipeline()
{
	return _pipeline;
}

void AEGLVideoWidget::setPipeline(AEImageProcessingPipeline *pipeline)
{
	if(_pipeline != NULL)
	{
		delete _pipeline;
	}
	_pipeline = pipeline;
}

void AEGLVideoWidget::initializeGL()
{	
	//Initialize local buffers
	GLuint tId[IMAGE_CACHE_SIZE];
	glGenTextures(IMAGE_CACHE_SIZE, tId);

	int i;
	for(i=0; i<IMAGE_CACHE_SIZE; i++)
	{
		_cvTextures[i].texId = tId[i];
		_cvTextures[i].initialized = 0;
		_cvTextures[i].texImage = NULL;
	}
	
	//Create a display list for drawing our texture.
	_rectList = glGenLists(1);

	glNewList(_rectList, GL_COMPILE);

	glBegin(GL_QUADS);
	GLfloat sz = 1.0;
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-sz, sz, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( sz, sz, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( sz,-sz, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-sz,-sz, 0.0f);
	glEnd();

	glEndList(); 

	_imageIndex = -1;
}

void AEGLVideoWidget::resizeGL(int w, int h)
{
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glViewport(0, 0, (GLfloat)w, (GLfloat)h);
	
	GLdouble fovy = 30.0;
	GLdouble aspect = (GLfloat)w/(GLfloat)h;
	GLdouble zNear = 1.0;
	GLdouble zFar = 1000.0;
	gluPerspective(fovy, aspect, zNear, zFar);
}

void AEGLVideoWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);

	glLoadIdentity();
	
	if(_imageIndex >= 0)
	{
		//See if we can kill an old image.
		int killIndex = (_imageIndex + (IMAGE_CACHE_SIZE - 1)) % IMAGE_CACHE_SIZE;
		struct cvTexture killTex = _cvTextures[killIndex];
		if(killTex.texImage != NULL)
		{
			cvReleaseImage(&(killTex.texImage));
			_cvTextures[killIndex].texImage = NULL;
			_cvTextures[killIndex].initialized = 0;
		}

		struct cvTexture texr = _cvTextures[_imageIndex];

		if(!(texr.initialized))
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texr.texId);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (texr.texImage)->width, (texr.texImage)->height, 
						0, GL_RGB, GL_UNSIGNED_BYTE, (texr.texImage)->imageData);
			texr.initialized = 1;
		}

		glTranslatef(0.0f, 0.0f,-1.0f);

		//Now just draw the texture using our display list.
		glCallList(_rectList);
	}
}

void AEGLVideoWidget::updateImage()
{
	//Grab the frame.
	IplImage *frame = _capture->captureFrame();
	
	//Make a copy of the frame for our own purposes
	//and convert color.
	IplImage *copy;
	if(_pipeline != NULL)
	{
		copy = _pipeline->processImage(frame);
		cvReleaseImage(&frame);
	}
	else
	{
		copy = frame;
	}
	
	cvCvtColor(copy, copy, CV_BGR2RGB);
	
	//Write the image into our buffer, it will take responsibility
	//for deallocation
	setImage(copy);
	
	//This will call paintGL (after setting the drawing context)
	updateGL();
}

void AEGLVideoWidget::setImage(IplImage *img)
{
	int newIndex = (_imageIndex + 1) % IMAGE_CACHE_SIZE;
	_cvTextures[newIndex].texImage = img;
	_imageIndex = newIndex;
}


