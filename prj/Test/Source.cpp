#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <assert.h>
#include <sstream>

#include "ImageJuggler/IJImageTranslator.h"
#include "ImageJuggler/Image/IJRGBImage.h"
#include "ImageJuggler/Image/IJYCbCrImage444.h"

#define	PROFILING_ENABLED 1
#include "Profiling.h"

#define DBG_REPORT_ERROR(format, ...) _CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, __FUNCDNAME__, format, __VA_ARGS__)
#define DBG_REPORT_WARN(format, ...) _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, __FUNCDNAME__, format, __VA_ARGS__)

void RGBLoadUnload()
{
	IJRGBImage* image = nullptr;
	{
		dbg__profileBlock2("Contructing IJRGBImage");
		image = new IJRGBImage("input/alp_tile_Cable_Bridge_df.tga");
	}

	IJResult result = IJResult::Success;
	
	{
		dbg__profileBlock2("Loading RGB Image");
		result = image->Load();
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Load error: %d", static_cast<int>(result));
		}
	}
	
	{
		dbg__profileBlock2("Save RGB image to vector<uint8_t>");
		std::vector<uint8_t> rawImage;
		result = image->Save(rawImage);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Save to raw image error: %d", static_cast<int>(result));
		}
	}

	{
		dbg__profileBlock2("RGB image save to file");
		result = image->Save("output/alp_tile_Cable_Bridge_df.tga");
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Save to file error: %d", static_cast<uint32_t>(result));
		}
	}

	{
		dbg__profileBlock2("Destructing RGB image");
		delete image;
		image = nullptr;
	}
}

void RGBToYCbCrTranslate()
{
	dbg__profileBlock();

	IJRGBImage* rgbImage = nullptr;
	IJYCbCrImage444* ybrImage = nullptr;
	IJResult result = IJResult::Success;

	{
		dbg__profileBlock2("Constructing RGB image");
		rgbImage = new IJRGBImage("input/alp_shadow_map_square4.tga");
		if (rgbImage == nullptr)
		{
			DBG_REPORT_ERROR("RGB imabe contruct failed");
		}
	}

	{
		dbg__profileBlock2("Loading RGB image");
		result = rgbImage->Load();
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("RGB image load failed: %d", static_cast<int>(result));
		}
	}

	{
		dbg__profileBlock2("Constructing IJYCbCr444 image");
		ybrImage = new IJYCbCrImage444();
		if (ybrImage == nullptr)
		{
			DBG_REPORT_ERROR("YCbCr image construct failed");
		}
	}

	{
		dbg__profileBlock2("Translation RGB -> YCbCr444");
		result = IJImageTranslator::RGBToYCbCr444(rgbImage, ybrImage);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("RGB -> YCbCr444 translation failed: %d", static_cast<int>(result));
		}
	}

	{
		dbg__profileBlock2("Save YCbCr to file");
		result = ybrImage->Save("output/ybr_alp_shadow_map_square4.tga");
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("YCbCr image save failed: %d", static_cast<int>(result));
		}
	}

	{
		dbg__profileBlock2("Destructing YCbCr image");
		delete ybrImage;
		ybrImage = nullptr;
	}

	{
		dbg__profileBlock2("Destructing RGB image");
		delete rgbImage;
		rgbImage = nullptr;
	}
}

void YCbCrSplitAndDumpSeparateChanlesTest()
{
	IJYCbCrImage444* image;
	IJRGBImage* yCompImage;
	IJRGBImage* bCompImage;
	IJRGBImage* rCompImage;
	IJResult result = IJResult::Success;

	{
		dbg__profileBlock2("Constructing YCbCrImage");
		image = new IJYCbCrImage444("output/ybr_alp_tile_Cable_Bridge_df.tga");
		if (image == nullptr)
		{
			DBG_REPORT_ERROR("YCbCr image contruction error");
			return;
		}
	}

	{
		dbg__profileBlock2("Loading image");
		result = image->Load();
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Loading YCbCr image failed: %d", static_cast<int>(result));
			return;
		}
	}

	{
		dbg__profileBlock2("Creating output image instances");
		
		yCompImage = new IJRGBImage();
		assert(yCompImage);
		
		bCompImage = new IJRGBImage();
		assert(bCompImage);

		rCompImage = new IJRGBImage();
		assert(rCompImage);
	}

	{
		dbg__profileBlock2("Spliting the ybr image to three rgb images");

		result = IJImageTranslator::YBRToRGBCompSlit(image, yCompImage, bCompImage, rCompImage);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("YBR split error %d", static_cast<int>(result));
		}
	}

	{
		dbg__profileBlock2("Clearing up data - Unload images");

		delete image;
		image = nullptr;

		delete yCompImage;
		yCompImage = nullptr;

		delete bCompImage;
		bCompImage = nullptr;

		delete rCompImage;
		bCompImage = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	//RGBLoadUnload();
	RGBToYCbCrTranslate();
	YCbCrSplitAndDumpSeparateChanlesTest();

	return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
