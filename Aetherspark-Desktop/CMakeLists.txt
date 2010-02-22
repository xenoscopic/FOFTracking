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

#Create tagets
ADD_LIBRARY(aedesktop SHARED ${library_sources})
ADD_EXECUTABLE(Aetherspark-Desktop main.cpp)

#Specify linkage
TARGET_LINK_LIBRARIES(aedesktop aerecognition)
TARGET_LINK_LIBRARIES(Aetherspark-Desktop aedesktop)

