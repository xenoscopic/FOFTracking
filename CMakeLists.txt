CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)
PROJECT(Aetherspark-Desktop)

#Build 32-bit on Mac OS X so we can use Quicktime for video input
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -arch i386")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch i386")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

#Object-Recognition Library
ADD_SUBDIRECTORY(libaerecognition)

#Aetherspark-Desktop App
ADD_SUBDIRECTORY(Aetherspark-Desktop)
