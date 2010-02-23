#pragma once

#include <QWidget>
#include <QMainWindow>
#include <QVBoxLayout>
#include "AEVideoWidget.h"

namespace Aetherspark
{
	namespace Desktop
	{
		class AEMainWindow : public QMainWindow
		{
			public:
				AEMainWindow(QWidget *parent = NULL);
		
			private:
				//Qt GUI controls
				QVBoxLayout *_layout;
				AEVideoWidget *_video;
		};
	}
}

