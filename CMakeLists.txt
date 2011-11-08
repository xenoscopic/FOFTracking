CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)
PROJECT(Aetherspark-Desktop)

#Add custom CMake modules
INCLUDE(/opt/local/share/opencv/OpenCVConfig.cmake)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	ADD_DEFINITIONS(-DMACOSX)
ENDIF()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	ADD_DEFINITIONS(-DWINDOWS)
ENDIF()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	ADD_DEFINITIONS(-DLINUX)
ENDIF()

#Tell CMake to put all libraries in build/lib
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

#Capture library
ADD_SUBDIRECTORY(libaecapture)

#Object-Recognition Library
ADD_SUBDIRECTORY(libaerecognition)

#Aetherspark-Desktop App
ADD_SUBDIRECTORY(Aetherspark-Desktop)
