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

	// Implementation
	ReturnValue extract_images(const std::string webm_filepath, std::shared_ptr<std::vector<Image> > sp_images, const int thread_count)
	{
		// Open webm file from harddrive
		WebMDemuxer demuxer(new MkvReader(webm_filepath.c_str()));
		if (demuxer.isOpen())
		{
			// Separate video material
			VPXDecoder videoDec(demuxer, thread_count);

			// Initialize frame and image from frame
			WebMFrame videoFrame;
			VPXDecoder::Image image;

			// Go over frames
			while (demuxer.readFrame(&videoFrame, NULL))
			{
				// Video frame
				if (videoDec.isOpen() && videoFrame.isValid())
				{
					if (!videoDec.decode(videoFrame))
					{
						break;
					}
					while (videoDec.getImage(image) == VPXDecoder::NO_ERROR)
					{
						// Get dimensions of the planes
						const int y_width = image.getWidth(0);
						const int y_height = image.getHeight(0);
						const int y_linesize = image.linesize[0];
						const int u_width = image.getWidth(1);
						const int u_height = image.getHeight(1);
						const int u_linesize = image.linesize[1];
						const int v_width = image.getWidth(2);
						const int v_height = image.getHeight(2);
						const int v_linesize = image.linesize[2];

						// Calculate sample of u and v
						const int u_steps_w = y_width / u_width;
						const int u_steps_h = y_height / u_height;
						const int v_steps_w = y_width / v_width;
						const int v_steps_h = y_height / v_height;

						// Push back new image into output
						simplewebm::Image output_image;
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
								const int y = *(image.planes[0] + (i * y_linesize) + j);
								const int u = *(image.planes[1] + (u_i * u_linesize) + u_j);
								const int v = *(image.planes[2] + (v_i * v_linesize) + v_j);

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

						// Move (!) image into output structure
						sp_images->emplace_back(std::move(output_image));
					}
				}
			}
			return ReturnValue::OK;
		}
		return ReturnValue::ERROR;
	}
}