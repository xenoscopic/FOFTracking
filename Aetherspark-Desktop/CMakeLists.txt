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

#Find Boost C++ libraries
FIND_PACKAGE(Boost 1.40)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

#Add Qt libraries/include directories
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	SET(QT_QMAKE_EXECUTABLE qmake-mac)
ENDIF()
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

#Create tagets
ADD_LIBRARY(aedesktop SHARED ${library_sources} ${library_MOC_SOURCES})
ADD_EXECUTABLE(Aetherspark main.cpp)

#Specify linkage
TARGET_LINK_LIBRARIES(aedesktop aerecognition aecapture ${QT_LIBRARIES})
TARGET_LINK_LIBRARIES(Aetherspark aedesktop ${QT_LIBRARIES})

