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

#Set up Qt Meta-Object Compiler sources
SET(library_MOD_HEADERS
	include/AEVideoWidget.h
)
QT4_WRAP_CPP(library_MOD_SOURCES ${library_MOC_HEADERS})

#Create tagets
ADD_LIBRARY(aedesktop SHARED ${library_sources} ${library_MOD_SOURCES})
ADD_EXECUTABLE(Aetherspark main.cpp)

#Specify linkage
TARGET_LINK_LIBRARIES(aedesktop aerecognition ${QT_LIBRARIES})
TARGET_LINK_LIBRARIES(Aetherspark aedesktop)

