#####################################################
#                                                   #
#                                                   #
#                                                   #
#  CMake Configuration for Aetherspark-Desktop App  #
#                                                   #
#                                                   #
#                                                   #
#####################################################

#Set library source directories
AUX_SOURCE_DIRECTORY(src library_sources)
INCLUDE_DIRECTORIES(${Aetherspark-Desktop_SOURCE_DIR}/Aetherspark-Desktop/include)
INCLUDE_DIRECTORIES(${Aetherspark-Desktop_SOURCE_DIR}/libaecapture/include)
INCLUDE_DIRECTORIES(${Aetherspark-Desktop_SOURCE_DIR}/libaerecognition/include)

#Add Qt libraries/include directories
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	SET(QT_QMAKE_EXECUTABLE qmake-mac)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTOPENGL TRUE) #Needed for QGLWidget
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
INCLUDE_DIRECTORIES(${QT_INCLUDES})

#Set up Qt Meta-Object Compiler sources
SET(library_MOC_HEADERS
	include/AEVideoWidget.h
	include/AEGLVideoWidget.h
)
QT4_WRAP_CPP(library_MOC_SOURCES ${library_MOC_HEADERS})

#Add cascade resources
SET(cascade_RCCS cascades.qrc)
QT4_ADD_RESOURCES(cascade_RCC_SRCS ${cascade_RCCS})

#Create tagets
ADD_LIBRARY(aedesktop SHARED ${library_sources} ${library_MOC_SOURCES})
ADD_EXECUTABLE(Aetherspark main.cpp ${cascade_RCC_SRCS})

#Specify linkage
TARGET_LINK_LIBRARIES(aedesktop aerecognition aecapture ${QT_LIBRARIES})
TARGET_LINK_LIBRARIES(Aetherspark aedesktop ${QT_LIBRARIES})

