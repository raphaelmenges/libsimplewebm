cmake_minimum_required(VERSION 2.8)

# Macro to use C++11 standard
macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 11)
  endif ()
endmacro(use_cxx11)
use_cxx11()

# Set project name
project(libsimplewebm)

# Decide where the VPX libraries are placed
set(SIMPLE_WEBM_USE_LOCAL_VPX ON CACHE BOOL "Link into local builds of VPX library.")

# Final libraries, linked in the end
set(FINAL_LIBRARIES "")

if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
	list(APPEND FINAL_LIBRARIES pthread)
endif()	

# libvpx
if(${SIMPLE_WEBM_USE_LOCAL_VPX}) # use local libs
	set(VPX_DIR ${CMAKE_CURRENT_LIST_DIR}/libvpx)
	if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
		if(${MSVC_VERSION} STREQUAL 1900) # MSVC2015
			set(VPX_RELEASE_LIBRARIES ${VPX_DIR}/lib/MSVC2015/vpxmd.lib)
			set(VPX_DEBUG_LIBRARIES ${VPX_DIR}/lib/MSVC2015/vpxmdd.lib)
		else() # MSVC2017
			set(VPX_RELEASE_LIBRARIES ${VPX_DIR}/lib/MSVC2017/vpxmd.lib)
			set(VPX_DEBUG_LIBRARIES ${VPX_DIR}/lib/MSVC2015/vpxmdd.lib)
		endif()
	elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU) # built on Debian 8.8
		set(VPX_RELEASE_LIBRARIES ${VPX_DIR}/lib/GCC/libvpx.a)
		set(VPX_DEBUG_LIBRARIES ${VPX_DIR}/lib/GCC/libvpx_g.a)
	endif()
	include_directories(${VPX_DIR}/include)
else() # use system libs (e.g., from apt-get)
	find_path(VPX_INCLUDE_DIRS
		NAMES vpx/vp8.h)
	find_library(VPX_LIBRARIES
		NAMES vpx)
	set(VPX_RELEASE_LIBRARIES ${VPX_LIBRARIES})
	set(VPX_DEBUG_LIBRARIES ${VPX_LIBRARIES})
	include_directories(${VPX_INCLUDE_DIRS})
endif()
	

# libwebm
include_directories(${CMAKE_CURRENT_LIST_DIR}/libwebm)

# Get own sources
set(SOURCES
	libsimplewebm.hpp
	src/libsimplewebm.cpp
	src/WebMDemuxer.cpp
	src/VPXDecoder.cpp
	src/OpusVorbisDecoder.cpp
	libwebm/mkvparser/mkvparser.cc)

# Create library
add_library(libsimplewebm STATIC ${SOURCES})

# Link libraries into executable
foreach(LIB ${VPX_RELEASE_LIBRARIES})
	target_link_libraries(libsimplewebm optimized ${LIB})
endforeach()
foreach(LIB ${VPX_DEBUG_LIBRARIES})
	target_link_libraries(libsimplewebm debug ${LIB})
endforeach()
foreach(LIB ${FINAL_LIBRARIES})
	target_link_libraries(libsimplewebm optimized ${LIB})
	target_link_libraries(libsimplewebm debug ${LIB})
endforeach()

# Create example
# add_executable(example example.cpp)
# target_link_libraries(example libsimplewebm)