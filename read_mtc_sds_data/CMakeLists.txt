#----------------------------------------------------------------------------
# CMake file for compiling program to convert MTC-SDS root file to text file.
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Setup project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(read_mtc_root_data CXX)

#----------------------------------------------------------------------------
# Find ROOT cmake config file "FindROOT.cmake".
set(CMAKE_MODULE_PATH $ENV{ROOT_INSTALL_DIR}/etc/cmake ${CMAKE_MODULE_PATH})
find_package(ROOT REQUIRED)

# Add search path prefix for mtc-sds.
set(CMAKE_PREFIX_PATH $ENV{MTCSDS_INSTALL_DIR} ${CMAKE_PREFIX_PATH})

#----------------------------------------------------------------------------
# Setup include directory for this project.
include_directories(${ROOT_INCLUDE_DIR} $ENV{MTCSDS_INSTALL_DIR}/include)

find_library(MTCSDS_LIBRARY_TARGET mtc-sds)

add_executable(${PROJECT_NAME} read_mtc_sds_data.cpp)

target_link_libraries(${PROJECT_NAME} ${ROOT_LIBRARIES}
	${MTCSDS_LIBRARY_TARGET})
