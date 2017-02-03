#pragma once

#include "IJTypes.h"

class IJRGBImage;
class IJYCbCrImage444;
class IJYCbCrImage422;

struct IJImageTranslator
{
	static IJResult RGBToYCbCr444(IJRGBImage*		input, IJYCbCrImage444* output);
	static IJResult RGBToYCbCr442(IJRGBImage*		input, IJYCbCrImage422* output);
	static IJResult YCbCr444ToRGB(IJYCbCrImage444*	input, IJRGBImage*		output);
	static IJResult YCbCr422ToRGB(IJYCbCrImage422*	input, IJRGBImage*		output);
};