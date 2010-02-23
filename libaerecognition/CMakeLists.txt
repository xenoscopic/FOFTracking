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
INCLUDE_DIRECTORIES(${Aetherspark-Desktop_SOURCE_DIR}/libaerecognition/include)

#Find Boost C++ libraries
FIND_PACKAGE(Boost 1.40)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

#Find OpenCV libraries
FIND_PACKAGE(OpenCV)
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIR})

#Create tagets
ADD_LIBRARY(aerecognition SHARED ${library_sources})

#Set linkage
TARGET_LINK_LIBRARIES(aerecognition ${OPENCV_LIBRARIES})
