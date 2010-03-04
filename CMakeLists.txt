CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)
PROJECT(Aetherspark-Desktop)


IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	ADD_DEFINITIONS(-DMACOSX)
	
	#Build 32-bit on Mac OS X so we can use Quicktime for video input
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -arch i386")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch i386")
ENDIF()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	ADD_DEFINITIONS(-DWINDOWS)
ENDIF()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	ADD_DEFINITIONS(-DLINUX)
ENDIF()

#FindBoost.cmake currently only searches to version 1.40
SET(Boost_ADDITIONAL_VERSIONS "1.41" "1.41.0" "1.42" "1.42.0")

#Capture library
ADD_SUBDIRECTORY(libaecapture)

#Object-Recognition Library
ADD_SUBDIRECTORY(libaerecognition)

#Aetherspark-Desktop App
ADD_SUBDIRECTORY(Aetherspark-Desktop)

