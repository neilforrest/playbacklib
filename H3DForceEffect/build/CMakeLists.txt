IF(WIN32)
  cmake_minimum_required(VERSION 2.6.0)
ENDIF(WIN32)

# The name of our project is "PlaybackEffect".  CMakeLists files in this project can
# refer to the root source directory of the project as ${PlaybackEffect_SOURCE_DIR} and
# to the root binary directory of the project as ${PlaybackEffect_BINARY_DIR}.
project (PlaybackEffect)

# Where to find Source files.
SET( PlaybackEffect_SRCS "${PlaybackEffect_SOURCE_DIR}/../src/PlaybackLibEffect.cpp"
						 "${PlaybackEffect_SOURCE_DIR}/../src/HAPIPlaybackLibEffect.cpp" )

# Add optional libs to this variable.
SET(optionalLibs)

# Include directories.
INCLUDE_DIRECTORIES( ../src )
INCLUDE_DIRECTORIES( ../include )

# Add required libs to this variable.
SET(requiredLibs)

# Where to find modules, used when finding packages.
SET(CMAKE_MODULE_PATH "$ENV{H3D_ROOT}/build/modules"
                      "${PlaybackEffect_SOURCE_DIR}/modules")
INCLUDE( StripAndAddLibraryDirectories )

# PlaybackLib
FIND_PACKAGE(PlaybackLib REQUIRED)

IF(PLAYBACKLIB_FOUND)
  INCLUDE_DIRECTORIES( ${PLAYBACKLIB_INCLUDE_DIR} )
  SET(requiredLibs ${requiredLibs} ${PLAYBACKLIB_LIBRARIES} )
ENDIF(PLAYBACKLIB_FOUND)

#H3DAPI
FIND_PACKAGE(H3DAPI REQUIRED)

IF(H3DAPI_FOUND)
  INCLUDE_DIRECTORIES( ${H3DAPI_INCLUDE_DIR} )
  SET(requiredLibs ${requiredLibs} ${H3DAPI_LIBRARIES} )
ENDIF(H3DAPI_FOUND)

#H3DUtil
FIND_PACKAGE(H3DUtil REQUIRED)

IF(H3DUTIL_FOUND)
  INCLUDE_DIRECTORIES( ${H3DUTIL_INCLUDE_DIR} ) 
  SET(requiredLibs ${requiredLibs} ${H3DUTIL_LIBRARIES} )
ENDIF(H3DUTIL_FOUND)

#HAPI
FIND_PACKAGE(HAPI REQUIRED)

IF(HAPI_FOUND)
  INCLUDE_DIRECTORIES( ${HAPI_INCLUDE_DIR} ) 
  SET(requiredLibs ${requiredLibs} ${HAPI_LIBRARIES} )
ENDIF(HAPI_FOUND)

# NF: Added this
# Optional extra library. Without this H3D API can not load scenes defined with X3D syntax.
FIND_PACKAGE(Xerces)
IF(XERCES_FOUND)
  SET(HAVE_XERCES 1)
  INCLUDE_DIRECTORIES( ${XERCES_INCLUDE_DIR} ) 
  SET(optionalLibs ${optionalLibs} ${XERCES_LIBRARIES} )
ENDIF(XERCES_FOUND)

# OpenGL
FIND_PACKAGE(OpenGL REQUIRED)

IF(OPENGL_FOUND)
  INCLUDE_DIRECTORIES( ${OPENGL_INCLUDE_DIR} ) 
  SET(requiredLibs ${requiredLibs} ${OPENGL_LIBRARIES} )
ENDIF(OPENGL_FOUND)

# Glut.
FIND_PACKAGE(GLUT REQUIRED)

IF( NOT GLUT_FOUND AND WIN32 )
  FIND_PACKAGE(GLUTWin REQUIRED)
ENDIF( NOT GLUT_FOUND AND WIN32 )

IF(GLUT_FOUND)
  INCLUDE_DIRECTORIES( ${GLUT_INCLUDE_DIR} )
  IF( WIN32 )
    STRIP_AND_ADD_LIBRARY_DIRECTORIES( ${GLUT_LIBRARIES} )
  ELSE( WIN32 )
    SET(requiredLibs ${requiredLibs} ${GLUT_LIBRARIES} )
  ENDIF( WIN32 )
ENDIF(GLUT_FOUND)

IF( NOT DEFINED BUILD_AS_LIB )
  SET( BUILD_AS_LIB "NO" CACHE BOOL "If set to YES the project is build as a library to use with H3DLoad or H3DViewer rather than a stand-alone executable." )
ENDIF( NOT DEFINED BUILD_AS_LIB )

# Define compile flags for all configurations, then add to them
SET( PlaybackEffect_COMPILE_FLAGS "" )

# Create build files.
IF ( BUILD_AS_LIB )

	# make sure symbols are exported by defining some symbols on the command line
	SET( PlaybackEffect_COMPILE_FLAGS "${PlaybackEffect_COMPILE_FLAGS} -DPLAYBACKEFFECT_EXPORTS -DBUILD_AS_LIBRARY" )

	ADD_LIBRARY ( PlaybackEffect SHARED ${PlaybackEffect_SRCS})

ELSE ( BUILD_AS_LIB )

	# Add additional source files for H3DLoad program
	SET( PlaybackEffect_SRCS "${PlaybackEffect_SRCS}" "${PlaybackEffect_SOURCE_DIR}/../src/H3DLoad.cpp" )

	ADD_EXECUTABLE(PlaybackEffect ${PlaybackEffect_SRCS})

ENDIF ( BUILD_AS_LIB )

TARGET_LINK_LIBRARIES( PlaybackEffect ${requiredLibs} ${optionalLibs} )

# Debug version should have _d postfix.
SET_TARGET_PROPERTIES( PlaybackEffect PROPERTIES DEBUG_POSTFIX "_d" )

# set the install directory to the H3D directory on Windows
IF(WIN32) 
  # Set compile and link properties for projects.
  IF(MSVC80)
    # Remove some warnings about deprecated functions. Do not treat wchar_t as built in type
    SET( PlaybackEffect_COMPILE_FLAGS "${PlaybackEffect_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE /Zc:wchar_t-" )
  ELSEIF(MSVC90)
    # Remove some warnings about deprecated functions. Do not treat wchar_t as built in type. Compile on multiple processors/threads if possible.
    SET( PlaybackEffect_COMPILE_FLAGS "${PlaybackEffect_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE /Zc:wchar_t- /MP" )
  ENDIF(MSVC80)

  IF( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
    SET( CMAKE_INSTALL_PREFIX ${PlaybackEffect_SOURCE_DIR}/.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
  ENDIF( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
ENDIF(WIN32)

# apply compile flags for project
SET_TARGET_PROPERTIES( PlaybackEffect PROPERTIES COMPILE_FLAGS "${PlaybackEffect_COMPILE_FLAGS}" )

# Install to this destination.
INSTALL( TARGETS PlaybackEffect
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin
	 ARCHIVE DESTINATION lib )

