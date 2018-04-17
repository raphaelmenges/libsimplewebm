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
		std::vector<char> data; // RGB pixels
	};

	// Image walker to fetch consecutive range of images from video
	class ImageWalker
	{
	public:

		// Destructor
		virtual ~ImageWalker() = 0;

		// Walk over video, return true when more video frames are available. 
		virtual bool walk(
			const unsigned int count_to_extract,
			std::shared_ptr<std::vector<Image> > sp_images,
			unsigned int * p_extracted_count = nullptr) = 0;

	protected:

		// Constructor
		ImageWalker();
		ImageWalker(const ImageWalker&) = delete;
		ImageWalker& operator=(ImageWalker const&) = delete;
	};

	// Factory of image walker
	std::unique_ptr<ImageWalker> create_image_walker(const std::string webm_filepath, const int thread_count = 1);
}