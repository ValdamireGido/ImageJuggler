#pragma once

#include "IJTypes.h"
#include "ImageJuggler_config.h"
#include <vector>

namespace image_juggler
{
	IJResult PackRGBImage(const std::vector<uint8_t>& rgb_input, std::vector<uint8_t>& output, uint8_t nComps, float packRate);
	IJResult PackRGBImage(const uint8_t* pRgb_input, size_t input_size, uint8_t* pYuv_output, size_t* pOutput_size, uint8_t nComps, float packRate);

	IJResult UnpackYUVImage(const std::vector<uint8_t>& yuv_input, std::vector<uint8_t>& rgb_output, uint8_t nComps, float packRate);
	IJResult UnpackYUVImage(const uint8_t* pYuv_input, size_t input_size, uint8_t* pRgb_output, size_t* pOutput_size, uint8_t nComps, float packRate);
}