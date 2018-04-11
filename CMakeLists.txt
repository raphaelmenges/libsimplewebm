cmake_minimum_required(VERSION 2.8)

# Set project name
project(simplewebm)

# Get dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(VPX REQUIRED vpx) # ${VPX_LIBRARIES}
pkg_check_modules(OPUS REQUIRED opus) # ${OPUS_LIBRARIES}
pkg_check_modules(VORBIS REQUIRED vorbis) # ${VORBIS_LIBRARIES}

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
target_link_libraries(simplewebm ${VPX_LIBRARIES} ${OPUS_LIBRARIES} ${VORBIS_LIBRARIES})