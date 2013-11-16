#----------------------------------------------------------------------------
# CMake file for compiling MTC Geant4 simulation program. Mich 2013/05/16
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
set(CMAKE_CXX_COMPILER g++)
project(mtcg4sim CXX)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Find cmake config files.
#
# Find ROOT cmake config file "FindROOT.cmake".
set(CMAKE_MODULE_PATH $ENV{ROOT_INSTALL_DIR}/etc/cmake ${CMAKE_MODULE_PATH})
find_package(ROOT REQUIRED)

# Add search path prefix and find cmake config files "cadmesh-config.cmake" and
# "assim-config.cmake".
set(CMAKE_PREFIX_PATH $ENV{CADMESH_INSTALL_DIR} ${CMAKE_PREFIX_PATH})
set(CMAKE_PREFIX_PATH $ENV{ASSIMP_INSTALL_DIR} ${CMAKE_PREFIX_PATH})
find_package(cadmesh REQUIRED)
find_package(assimp REQUIRED)

# Add search path prefix for mtc-sds.
set(CMAKE_PREFIX_PATH $ENV{MTCSDS_INSTALL_DIR} ${CMAKE_PREFIX_PATH})

#----------------------------------------------------------------------------
# Add g++ flags.
#
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall -Wl,-V")

# Setup include directory for this project
#
include_directories(
    ${PROJECT_SOURCE_DIR}/include
	${Geant4_INCLUDE_DIRS}
    ${ROOT_INCLUDE_DIR}
	$ENV{CADMESH_INSTALL_DIR}/include # Use environment variable CADMESH_INCLUDE_DIR.
	${ASSIMP_INCLUDE_DIRS}
	$ENV{MTCSDS_INSTALL_DIR}/include # Use environment variable MTC_SDS_INCLUDE_DIR.
	)

# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/mtcg4sim.cc ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Find libraries.
set(ROOT_LIBRARIES ${ROOT_LIBRARIES}
	Core
    Cint
    RIO
    Net
    Hist
    Graf
    Graf3d
    Gpad
    Tree
    Rint
    Postscript
    Matrix
    Physics
    MathCore
    Thread
    m
    dl)
# Find assimp libraries.
find_library(ASSIMP_LIBRARY_TARGET ${ASSIMP_LIBRARIES})

# Find mtc-sds library libmtc-sds.so.
find_library(MTCSDS_LIBRARY_TARGET mtc-sds)

# Find cadmesh library libcadmesh.so.
find_library(CADMESH_LIBRARY_TARGET cadmesh)

#----------------------------------------------------------------------------
# Add the executable, and link it to libraries
#
add_executable(${PROJECT_NAME} ${sources} ${headers})
target_link_libraries(${PROJECT_NAME}
    ${Geant4_LIBRARIES}
    ${ROOT_LIBRARIES}
	${CADMESH_LIBRARY_TARGET}
	${ASSIMP_LIBRARY_TARGET}
	${MTCSDS_LIBRARY_TARGET}
    )

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build MTCG4Sim. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
#set(EXAMPLE_MTCG4SIM_SCRIPTS
#    vis.mac
#    run.mac
#  )
#
#foreach(_script ${EXAMPLE_MTCG4SIM_SCRIPTS})
#  configure_file(
#    ${PROJECT_SOURCE_DIR}/${_script}
#    ${PROJECT_BINARY_DIR}/${_script}
#    COPYONLY
#    )
#endforeach()

#----------------------------------------------------------------------------
# Copy all other relavent files to the build directory, i.e. the
# directory in which we # build MTCG4Sim.
#

# Copy MTC CAD files.
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_SOURCE_DIR}/mtc_cad/ ${CMAKE_BINARY_DIR}/mtc_cad/)

# Copy input data files.
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_SOURCE_DIR}/input/ ${CMAKE_BINARY_DIR}/input/)

# Copy macro files.
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_SOURCE_DIR}/mac/ ${CMAKE_BINARY_DIR}/mac/)

# Copy tools.
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_SOURCE_DIR}/tools/ ${CMAKE_BINARY_DIR}/tools/)

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
#add_custom_target(MTCG4Sim DEPENDS mtc)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
#install(TARGETS ${PROJECT_NAME} DESTINATION bin)

#----------------------------------------------------------------------------
# Specify build type for specific compiler flags.
# E.g. "Release" build type will set compiler optimization flags.
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)
