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
FIND_PACKAGE(Boost 1.45)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

#Find OpenCV libraries
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

#Create tagets
ADD_LIBRARY(aerecognition SHARED ${library_sources})

#Set linkage
TARGET_LINK_LIBRARIES(aerecognition ${OpenCV_LIBS})
