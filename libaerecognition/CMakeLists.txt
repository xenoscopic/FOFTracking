#####################################################
#                                                   #
#                                                   #
#                                                   #
#     CMake Configuration for libaerecognition      #
#                                                   #
#                                                   #
#                                                   #
#####################################################

#Set source directories
AUX_SOURCE_DIRECTORY(src library_sources)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

#Find Boost C++ libraries
FIND_PACKAGE(Boost 1.40)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

#Find OpenCV libraries
FIND_PACKAGE(OpenCV)
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIR})

#Create tagets
ADD_LIBRARY(aerecognition SHARED ${library_sources})

#Set linkage
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	#Hack until we have a working FindOpenCV.cmake for OpenCV 2.0
	SET(OPENCV_LIBRARIES 
		cv200 
		cvaux200 
		highgui200 
		cxcore200 
		ml200 
		opencv_ffmpeg200 
		cxts200
	)
ENDIF()
TARGET_LINK_LIBRARIES(aerecognition ${OPENCV_LIBRARIES})
