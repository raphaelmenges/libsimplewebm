/*
*    MIT License
*
*    Copyright (c) 2018 Raphael Menges
*
*    Permission is hereby granted, free of charge, to any person obtaining a copy
*    of this software and associated documentation files (the "Software"), to deal
*    in the Software without restriction, including without limitation the rights
*    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*    copies of the Software, and to permit persons to whom the Software is
*    furnished to do so, subject to the following conditions:
*
*    The above copyright notice and this permission notice shall be included in all
*    copies or substantial portions of the Software.
*
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*    SOFTWARE.
*/

#include <vector>
#include <memory>

namespace simplewebm
{
	// Simple image class to hold data of one frame from movie
	class Image
	{
	public:
		int width = 0;
		int height = 0;
		std::vector<char> data; // BGR pixels
		double time = 0.0; // Frame time in seconds
	};

	// Video walker to fetch consecutive range of images from video
	class VideoWalker
	{
	public:

		// Destructor
		virtual ~VideoWalker() = 0;

		// Walk over video, return true when more video frames are available. count_to_extract == 0 will walk over complete video.
		virtual bool walk(
			std::shared_ptr<std::vector<Image> > sp_images,
			const unsigned int count_to_extract = 0,
			unsigned int * p_extracted_count = nullptr) = 0;

		// Dry walk over the video, to gather frame times. count_to_extract == 0 will walk over complete video.
		virtual bool dry_walk(
			std::shared_ptr<std::vector<double> > sp_times,
			const unsigned int count_to_extract = 0,
			unsigned int * p_extracted_count = nullptr) = 0;

	protected:

		// Constructor
		VideoWalker();
		VideoWalker(const VideoWalker&) = delete;
		VideoWalker& operator=(VideoWalker const&) = delete;
	};

	// Factory of video walker
	std::unique_ptr<VideoWalker> create_video_walker(const std::string webm_filepath, const int thread_count = 1);
}