#####################################################
#                                                   #
#                                                   #
#                                                   #
#       CMake Configuration for libaecapture        #
#                                                   #
#                                                   #
#                                                   #
#####################################################

#Set source directories
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	SET(library_sources src/AECaptureDeviceMac.mm)
	SET(CMAKE_SHARED_LINKER_FLAGS "-framework Foundation -framework QTKit -framework CoreVideo")
ENDIF()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	SET(library_sources src/AECaptureDeviceWin.cpp)
ENDIF()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	SET(library_sources src/AECaptureDeviceLinux.cpp)
ENDIF()

#Set include directories
INCLUDE_DIRECTORIES(${Aetherspark-Desktop_SOURCE_DIR}/libaecapture/include)

#Find OpenCV libraries
FIND_PACKAGE(OpenCV)
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIR})

#Create tagets
ADD_LIBRARY(aecapture SHARED ${library_sources})

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
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	TARGET_LINK_LIBRARIES(aecapture ${OPENCV_LIBRARIES})
ELSE()
	TARGET_LINK_LIBRARIES(aecapture ${OPENCV_LIBRARIES})
ENDIF()
