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

#include "libsimplewebm.h"
#include "bitmap/bitmap_image.hpp"
#include <sstream>

#include <iostream>

int main()
{
	auto frames = simplewebm::extract_frames("C:/recording.webm");

	// Go over frames and output them as bitmap
	int frameNumber = 0;
	for (const auto& frame : *frames.get())
	{
		// Initialize output bitmap for the frame
		bitmap_image output(frame.width, frame.height);

		// Go over pixels
		for (int i = 0; i < frame.height; ++i)
		{
			for (int j = 0; j < frame.width; ++j)
			{
				// Calculate index in data of frame
				int data_index = (i * frame.width) + j;
				data_index *= 3;

				// Set pixel in output bitmap
				output.set_pixel(
					j, i,
					frame.data.at(data_index),
					frame.data.at(data_index+1),
					frame.data.at(data_index+2));
			}
		}

		// Store output
		std::ostringstream s;
		s << frameNumber;
		output.save_image("output_" + s.str() + ".bmp");
		++frameNumber;
	}

	return 0;
}
