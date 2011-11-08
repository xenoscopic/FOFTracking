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
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

#Find OpenCV libraries
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

#Create tagets
ADD_LIBRARY(aecapture SHARED ${library_sources})

#Set linkage
TARGET_LINK_LIBRARIES(aecapture ${OpenCV_LIBS})
