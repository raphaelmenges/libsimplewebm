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
	// Return values
	enum class ReturnValue {
		OK,
		ERROR
	};

	// Simple image class to hold data of one frame from movie
	class Image
	{
	public:
		int width;
		int height;
		std::vector<char> data; // RGB pixels
	};

	// Add all frames of a movie to shared vector of images (shared vector is *not* cleared)
	ReturnValue extract_frames(const std::string webm_filepath, std::shared_ptr<std::vector<Image> > sp_images, const int thread_count = 1);
}