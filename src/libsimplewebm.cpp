/*
*    MIT License
*
*    Copyright (c) 2018 Raphael Menges
*
*    Copyright (c) 2016 Błażej Szczygieł
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

#include "../libsimplewebm.h"
#include "VPXDecoder.hpp"
#include "mkvparser/mkvparser.h"
#include <sstream>
#include <string>
#include <algorithm>

namespace simplewebm
{
	/////////////////////////////////////////////////
	/// Helpers
	/////////////////////////////////////////////////

	// Class to read file
	class MkvReader : public mkvparser::IMkvReader
	{
	public:
		MkvReader(const char *filePath) :
			m_file(fopen(filePath, "rb"))
		{}
		~MkvReader()
		{
			if (m_file)
				fclose(m_file);
		}

		int Read(long long pos, long len, unsigned char *buf)
		{
			if (!m_file)
				return -1;
			fseek(m_file, pos, SEEK_SET);
			const size_t size = fread(buf, 1, len, m_file);
			if (size < size_t(len))
				return -1;
			return 0;
		}
		int Length(long long *total, long long *available)
		{
			if (!m_file)
				return -1;
			const off_t pos = ftell(m_file);
			fseek(m_file, 0, SEEK_END);
			if (total)
				*total = ftell(m_file);
			if (available)
				*available = ftell(m_file);
			fseek(m_file, pos, SEEK_SET);
			return 0;
		}

	private:
		FILE * m_file;
	};

	// Function to clamp int within range of char
	inline int clamp8(int v)
	{
		return std::min(std::max(v, 0), 255);
	}

	/////////////////////////////////////////////////
	/// ImageWalkerImpl Declaration
	/////////////////////////////////////////////////

	// Implementation of image walker class
	class ImageWalkerImpl : public ImageWalker
	{
	public:

		// Constructor
		ImageWalkerImpl(const std::string webm_filepath, const int thread_count);
		ImageWalkerImpl(const ImageWalker&) = delete;
		ImageWalkerImpl& operator=(ImageWalker const&) = delete;

		// Destructor
		virtual ~ImageWalkerImpl();

		// Walk over video, return true when more video frames are available. 
		virtual bool walk(
			const unsigned int count_to_extract,
			std::shared_ptr<std::vector<Image> > sp_images,
			unsigned int * p_extracted_count = nullptr);

	private:

		// Members
		std::unique_ptr<WebMDemuxer> _up_webm_demuxer = nullptr; // splits video and audio
		std::unique_ptr<WebMFrame> _up_webm_frame = nullptr; // holds encoded video frame
		std::unique_ptr<VPXDecoder> _up_vpx_decoder = nullptr; // decods video frame
		VPXDecoder::Image _vpx_image; // decoded video frame
	};

	/////////////////////////////////////////////////
	/// ImageWalker factory definition
	/////////////////////////////////////////////////

	// Factory of image walkers
	std::unique_ptr<ImageWalker> create_image_walker(const std::string webm_filepath, const int thread_count)
	{
		return std::unique_ptr<ImageWalker>(new ImageWalkerImpl(webm_filepath, thread_count));
	}

	/////////////////////////////////////////////////
	/// ImageWalkerImpl Definition
	/////////////////////////////////////////////////

	// Constructor of base class
	ImageWalker::ImageWalker()
	{
		// Do nothing
	}

	// Destructor of base class
	ImageWalker::~ImageWalker()
	{
		// Do nothing
	}

	// Constructor
	ImageWalkerImpl::ImageWalkerImpl(const std::string webm_filepath, const int thread_count) : ImageWalker()
	{
		// Create WebMDemuxer while opening video file
		_up_webm_demuxer = std::unique_ptr<WebMDemuxer>(new WebMDemuxer(new MkvReader(webm_filepath.c_str())));

		// Continue when demuxer could be initialized
		if (_up_webm_demuxer->isOpen())
		{
			// Initialize further members
			_up_webm_frame = std::unique_ptr<WebMFrame>(new WebMFrame);
			_up_vpx_decoder = std::unique_ptr<VPXDecoder>(new VPXDecoder(*_up_webm_demuxer.get(), thread_count));
		}
		else
		{
			_up_webm_demuxer = nullptr;
		}
	}

	// Destructor
	ImageWalkerImpl::~ImageWalkerImpl()
	{
		// Do nothing
	}

	// Walk over video, return true when more video frames are available
	bool ImageWalkerImpl::walk(
		const unsigned int count_to_extract,
		std::shared_ptr<std::vector<Image> > sp_images,
		unsigned int * p_extracted_count)
	{
		// Check whether demuxer object has been correctly initialized
		if (_up_webm_demuxer)
		{
			// Go over frames
			unsigned int i = 0;
			bool frames_left = true;
			while (frames_left && i < count_to_extract)
			{
				// Read next frame
				if (
					_up_webm_demuxer->readFrame(_up_webm_frame.get(), NULL) // get video frame, only
					&& _up_webm_frame->isValid() // check frame for validity
					&& _up_vpx_decoder->isOpen() // check whether decoder is still open
					&& _up_vpx_decoder->decode(*_up_webm_frame.get())) // decode frame
				{
					// Get image of decoded video frame
					simplewebm::Image output_image;
					if (_up_vpx_decoder->getImage(_vpx_image) == VPXDecoder::NO_ERROR)
					{
						// Get dimensions of the planes
						const int y_width = _vpx_image.getWidth(0);
						const int y_height = _vpx_image.getHeight(0);
						const int y_linesize = _vpx_image.linesize[0];
						const int u_width = _vpx_image.getWidth(1);
						const int u_height = _vpx_image.getHeight(1);
						const int u_linesize = _vpx_image.linesize[1];
						const int v_width = _vpx_image.getWidth(2);
						const int v_height = _vpx_image.getHeight(2);
						const int v_linesize = _vpx_image.linesize[2];

						// Calculate sample of u and v
						const int u_steps_w = y_width / u_width;
						const int u_steps_h = y_height / u_height;
						const int v_steps_w = y_width / v_width;
						const int v_steps_h = y_height / v_height;

						// Push back new image into output
						output_image.width = y_width;
						output_image.height = y_height;
						output_image.data.reserve(y_width * y_height * 3); // RGB

						// Iterate over y plane
						for (int i = 0; i < y_height; ++i)
						{
							for (int j = 0; j < y_width; ++j)
							{
								// Calculate index for u and v
								const int u_i = i / u_steps_h;
								const int u_j = j / u_steps_w;
								const int v_i = i / v_steps_h;
								const int v_j = j / v_steps_w;

								// Extract YUV color of pixel
								const int y = *(_vpx_image.planes[0] + (i * y_linesize) + j);
								const int u = *(_vpx_image.planes[1] + (u_i * u_linesize) + u_j);
								const int v = *(_vpx_image.planes[2] + (v_i * v_linesize) + v_j);

								// Convert YUV to RGB
								const int c = y - 16;
								const int d = u - 128;
								const int e = v - 128;
								const char r = clamp8((298 * c + 409 * e + 128) >> 8);
								const char g = clamp8((298 * c - 100 * d - 208 * e + 128) >> 8);
								const char b = clamp8((298 * c + 516 * d + 128) >> 8);

								// Set pixel value
								output_image.data.push_back(r);
								output_image.data.push_back(g);
								output_image.data.push_back(b);
							}
						}
					}

					// Move (!) image into output structure
					sp_images->emplace_back(std::move(output_image));

					// Increase count of extracted frames
					++i;
				}
				else
				{
					frames_left = false;
				}
			}

			// Provide ouput
			if (p_extracted_count)
			{
				*p_extracted_count = i;
			}
			return frames_left;
		}
		else
		{
			// Provide ouput
			if (p_extracted_count)
			{
				*p_extracted_count = 0;
			}
			return false;
		}
	}
}