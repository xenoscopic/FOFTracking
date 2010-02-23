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
INCLUDE_DIRECTORIES(${Aetherspark-Desktop_SOURCE_DIR}/libaerecognition/include)

#Add Qt libraries/include directories
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

#Find Boost C++ libraries
FIND_PACKAGE(Boost 1.40)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

#Create tagets
ADD_LIBRARY(aedesktop SHARED ${library_sources})
ADD_EXECUTABLE(Aetherspark main.cpp)

#Specify linkage
TARGET_LINK_LIBRARIES(aedesktop aerecognition)
TARGET_LINK_LIBRARIES(Aetherspark aedesktop)

