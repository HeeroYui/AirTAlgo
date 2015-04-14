cmake_minimum_required(VERSION 2.8.3)
project(audio_drain)

set(CMAKE_VERBOSE_MAKEFILE ON)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  etk
  audio
  )

find_library(LIB_SPEEX_DSP libspeexdsp.a)
find_library(SPEEX_DSP speexdsp)

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if you package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
  INCLUDE_DIRS ../
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS etk audio
  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  ..
  ${speex_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
)

## Declare a cpp library
add_library(${PROJECT_NAME}
  ../audio/drain/debug.cpp
  ../audio/drain/airtalgo.cpp
  ../audio/drain/Algo.cpp
  ../audio/drain/ChannelReorder.cpp
  ../audio/drain/CircularBuffer.cpp
  ../audio/drain/EndPointCallback.cpp
  ../audio/drain/EndPoint.cpp
  ../audio/drain/EndPointRead.cpp
  ../audio/drain/EndPointWrite.cpp
  ../audio/drain/FormatUpdate.cpp
  ../audio/drain/Process.cpp
  ../audio/drain/Resampler.cpp
  ../audio/drain/Volume.cpp
  ../audio/drain/IOFormatInterface.cpp
  ../audio/drain/AutoLogInOut.cpp
)

#message("libspeex DSP file: ${LIB_SPEEX_DSP}")
#message("libspeex DSP file: ${SPEEX_DSP}")

add_definitions(-DHAVE_SPEEX_DSP_RESAMPLE)

## Specify libraries to link a library or executable target against
target_link_libraries(${PROJECT_NAME}
  ${SPEEX_DSP}
  ${catkin_LIBRARIES}
)

#############
## Install ##
#############

## Mark executables and/or libraries for installation
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
)

## Mark cpp header files for installation
install(DIRECTORY ../audio/drain/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
  FILES_MATCHING PATTERN "*.h"
)
