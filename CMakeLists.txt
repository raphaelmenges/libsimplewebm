cmake_minimum_required(VERSION 2.8)

# Set project name
project(libsimplewebm)

# Activate C++11 standard
if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
endif(NOT MSVC)

# Final libraries, linked in the end
set(FINAL_LIBRARIES "")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	list(APPEND FINAL_LIBRARIES pthread)
	# list(APPEND FINAL_LIBRARIES dl)
endif()	

# libvpx
set(VPX_DIR ${CMAKE_SOURCE_DIR}/libvpx)
if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)

  if(${MSVC_VERSION} STREQUAL 1900) # MSVC2015
	set(VPX_RELEASE_LIBRARIES ${VPX_DIR}/lib/MSVC2015/vpxmd.lib)
	set(VPX_DEBUG_LIBRARIES ${VPX_DIR}/lib/MSVC2015/vpxmdd.lib)
  else() # MSVC2017
	set(VPX_RELEASE_LIBRARIES ${VPX_DIR}/lib/MSVC2017/vpxmd.lib)
	set(VPX_DEBUG_LIBRARIES ${VPX_DIR}/lib/MSVC2015/vpxmdd.lib)
  endif()
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
	set(VPX_RELEASE_LIBRARIES ${VPX_DIR}/lib/GCC/libvpx.a)
	set(VPX_DEBUG_LIBRARIES ${VPX_DIR}/lib/GCC/libvpx_g.a)
endif()
include_directories(${VPX_DIR}/include)

include_directories(${CMAKE_SOURCE_DIR}/libwebm)

# Get sources
set(SOURCES
	libsimplewebm.h
	libsimplewebm.cpp
	WebMDemuxer.cpp
	VPXDecoder.cpp
	OpusVorbisDecoder.cpp
	libwebm/mkvparser/mkvparser.cc)

# Create library
add_library(libsimplewebm STATIC ${SOURCES})

# Link libraries into executable
target_link_libraries(libsimplewebm optimized ${VPX_RELEASE_LIBRARIES} ${FINAL_LIBRARIES})
target_link_libraries(libsimplewebm debug ${VPX_DEBUG_LIBRARIES} ${FINAL_LIBRARIES})

# Create example
# add_executable(example example.cpp)
# target_link_libraries(example libsimplewebm)