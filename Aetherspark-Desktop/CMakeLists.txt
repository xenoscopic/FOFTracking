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
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libaecapture/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libaerecognition/include)

#Find Boost C++ libraries
FIND_PACKAGE(Boost 1.40)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

#Add Qt libraries/include directories
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	SET(QT_QMAKE_EXECUTABLE qmake-mac)
ENDIF()

#If we're on the Mac, we need CoreFoundation to manage
#bundle resources
IF(APPLE)
	FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation)
ELSE()
	SET(COREFOUNDATION_LIBRARY)
ENDIF()

#If we're on Windows, we need Qt to build the
#winmain@16 call.  This will be provided in qtmain.lib.
IF(WIN32)
	SET(QT_USE_QTMAIN TRUE)
ENDIF()

FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTOPENGL TRUE) #Needed for QGLWidget
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
INCLUDE_DIRECTORIES(${QT_INCLUDES})

#Set up Qt Meta-Object Compiler sources
SET(library_MOC_HEADERS
	include/AEVideoWidget.h
	include/AEGLVideoWidget.h
)
QT4_WRAP_CPP(library_MOC_SOURCES ${library_MOC_HEADERS})

SET(aetherspark_SOURCES
	main.cpp
)

#Set the App icon file if on Mac OS X
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	# set how it shows up in the Info.plist file
	SET(MACOSX_BUNDLE_ICON_FILE Aetherspark.icns) 
	# set where in the bundle to put the icns file
	SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/Aetherspark.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	# include the icns file in the target
	SET(aetherspark_SOURCES ${aetherspark_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/Aetherspark.icns)
ENDIF()

#Create tagets
ADD_LIBRARY(aedesktop SHARED ${library_sources} ${library_MOC_SOURCES})
ADD_EXECUTABLE(Aetherspark MACOSX_BUNDLE WIN32 ${aetherspark_SOURCES})

#Specify linkage
TARGET_LINK_LIBRARIES(aedesktop aerecognition aecapture ${QT_LIBRARIES} ${COREFOUNDATION_LIBRARY})
TARGET_LINK_LIBRARIES(Aetherspark aedesktop ${QT_LIBRARIES})

#Set up installation directories
SET(plugin_dest_dir bin)
SET(qtconf_dest_dir bin)
SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/Aetherspark")
IF(APPLE)
	SET(plugin_dest_dir Aetherspark.app/Contents/MacOS)
	SET(qtconf_dest_dir Aetherspark.app/Contents/Resources)
	SET(frameworks_dest_dir Aetherspark.app/Contents/Frameworks)
	SET(cascades_dest_dir Aetherspark.app/Contents/Resources/Cascades)
	SET(APPS "\${CMAKE_INSTALL_PREFIX}/Aetherspark.app")
ENDIF(APPLE)
IF(WIN32)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/Aetherspark.exe")
ENDIF(WIN32)

# Install the Aetherspark application, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.
INSTALL(TARGETS Aetherspark 
	BUNDLE DESTINATION . COMPONENT Runtime
	RUNTIME DESTINATION bin COMPONENT Runtime
)

# Install needed Qt plugins by copying directories from the qt installation
# One can cull what gets copied by using 'REGEX "..." EXCLUDE'
INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)

# install a qt.conf file
# this inserts some cmake code into the install script to write the file
INSTALL(CODE "
    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")
    " COMPONENT Runtime)

# Use BundleUtilities to get all other dependencies for the application to work.
# It takes a bundle or executable along with possible plugins and inspects it
# for dependencies.  If they are not system dependencies, they are copied.

# directories to look for dependencies
SET(DIRS
	${QT_LIBRARY_DIRS}
	"/usr/local/lib"
	${CMAKE_BINARY_DIR}/lib
)

# Now the work of copying dependencies into the bundle/package
# The quotes are escaped and variables to use at install time have their $ escaped
# An alternative is the do a configure_file() on a script and use install(SCRIPT  ...).
# Note that the image plugins depend on QtSvg and QtXml, and it got those copied
# over.
INSTALL(CODE "
    file(GLOB_RECURSE QTPLUGINS
      \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    include(BundleUtilities)
    fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
    " COMPONENT Runtime)

#Copy the qt_menu.nib resource.  See these links for more info:
#	(In order of decreasing usefulness)
#	http://www.cmake.org/pipermail/cmake/2009-December/033743.html
#	http://markmail.org/thread/eneuve2yoemzube7
#	http://www.mail-archive.com/paraview@paraview.org/msg05234.html
#	http://www.paraview.org/pipermail/paraview/2009-August/013085.html
#
#Apparently this not being copied is a known bug.
#Also, the cp command doesn't seem to work properly when copying the nib (it only copies)
#its contents (even if I don't put the trailing slash).  So instead of I have to create
#the directory first and then copy the nib contents.  I have no idea why.
IF(APPLE)
INSTALL(CODE "
	file(MAKE_DIRECTORY
		\"\${CMAKE_INSTALL_PREFIX}/${frameworks_dest_dir}/QtGui.framework/Resources/qt_menu.nib\")
	execute_process(COMMAND cp -R 
		\"${QT_LIBRARY_DIR}/QtGui.framework/Resources/qt_menu.nib/\" 
		\"\${CMAKE_INSTALL_PREFIX}/${frameworks_dest_dir}/QtGui.framework/Resources/qt_menu.nib\")
	" COMPONENT Runtime)
ENDIF()

#Copy all cascades into the resources directory
IF(APPLE)
INSTALL(CODE "
	file(MAKE_DIRECTORY
		\"\${CMAKE_INSTALL_PREFIX}/${cascades_dest_dir}\")
	execute_process(COMMAND cp -R
		\"${CMAKE_CURRENT_SOURCE_DIR}/cascades/\" 
		\"\${CMAKE_INSTALL_PREFIX}/${cascades_dest_dir}\")
	" COMPONENT Runtime)
ENDIF()

# To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
# where CPackConfig.cmake is created by including CPack
# And then there's ways to customize this as well
IF(UNIX AND NOT APPLE)
	SET(CPACK_GENERATOR "DEB")
ENDIF()
IF(APPLE)
	SET(CPACK_GENERATOR "DragNDrop")
ENDIF()
IF(WIN32)
	SET(CPACK_GENERATOR "NSIS")
ENDIF()
INCLUDE(CPack)
