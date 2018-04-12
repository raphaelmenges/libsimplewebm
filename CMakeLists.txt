cmake_minimum_required(VERSION 2.8)

# Set project name
project(simplewebm)

# Libvpx TODO: link according to generator
set(VPX_LIBRARIES ${CMAKE_SOURCE_DIR}/libvpx/lib/MSVC2015/vpxmdd.lib)
include_directories(libvpx/include)

# Get sources
set(SOURCES
	example.cpp
	WebMDemuxer.cpp
	VPXDecoder.cpp
	OpusVorbisDecoder.cpp
	libwebm/mkvparser/mkvparser.cc)

# Include paths
include_directories(libwebm)

# Create executable
add_executable(simplewebm ${SOURCES})

# Link libraries into executable
target_link_libraries(simplewebm ${VPX_LIBRARIES})