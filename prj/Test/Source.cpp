#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <assert.h>
#include <sstream>

#include "ImageJuggler/IJImageTranslator.h"
#include "ImageJuggler/Image/IJRGBImage.h"
#include "ImageJuggler/Image/IJYCbCrImage888.h"

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

	IJResult result = IJResult::UnknownResult;
	
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
	IJYCbCrImage888* ybrImage = nullptr;
	IJResult result = IJResult::UnknownResult;

	const std::string inputFileName  = "input/bar_skybox.tga";
	const std::string outputFileName = "output/bar_skybox.ybr.tga";

	{
		dbg__profileBlock2("Loading RGB image");
		rgbImage = new IJRGBImage(inputFileName);
		if (rgbImage == nullptr)
		{
			DBG_REPORT_ERROR("RGB imabe contruct failed");
		}

		result = rgbImage->Load();
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("RGB image load failed: %d", static_cast<int>(result));
		}
	}

	{
		dbg__profileBlock2("Translation RGB -> YCbCr444");
		ybrImage = new IJYCbCrImage888();
		assert(ybrImage);
		if (ybrImage == nullptr)
		{
			DBG_REPORT_ERROR("YCbCr image construct failed");
		}

		result = IJImageTranslator::RGBToYCbCr444(rgbImage, ybrImage);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("RGB -> YCbCr444 translation failed: %d", static_cast<int>(result));
		}
	}

	{
		dbg__profileBlock2("Save YCbCr to file");
		result = ybrImage->Save(outputFileName);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("YCbCr image save failed: %d", static_cast<int>(result));
		}

		delete ybrImage;
		ybrImage = nullptr;

		delete rgbImage;
		rgbImage = nullptr;
	}
}

void YBRToRGBTranslate()
{
	dbg__profileBlock();

	IJYCbCrImage888* ybrImage = nullptr;
	IJRGBImage* rgbImage	  = nullptr;
	IJResult result			  = IJResult::UnknownResult;

	const std::string inputFileName  = "output/bar_skybox.ybr.tga";
	const std::string outputFileName = "output/bar_skybox.rgb.tga";

	{
		dbg__profileBlock2("Loading YBR Image");

		ybrImage = new IJYCbCrImage888();
		assert(ybrImage);
		if (!ybrImage)
		{
			DBG_REPORT_ERROR("Error allocating IJYCbCrImage444");
			return;
		}

		result = ybrImage->Load(inputFileName);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Error loading ybr input image %d", static_cast<int>(result));
		}
	}


	{
		dbg__profileBlock2("Converting YBR -> RGB");

		rgbImage = new IJRGBImage();
		assert(rgbImage);
		if (!rgbImage)
		{
			result = IJResult::MemoryAllocationError;
			DBG_REPORT_ERROR("Error allocating IJRGBImage %d", static_cast<int>(result));
		}

		result = IJImageTranslator::YCbCr444ToRGB(ybrImage, rgbImage);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Error translating ybr to rgb %d", static_cast<int>(result));
		}
	}


	{
		dbg__profileBlock2("Saving and deallocating");

		if (rgbImage)
		{
			result = rgbImage->Save(outputFileName);
			if (result != IJResult::Success)
			{
				DBG_REPORT_ERROR("Error saving output rgb image %d", static_cast<int>(result));
			}

			delete rgbImage;
			rgbImage = nullptr;
		}

		delete ybrImage;
		ybrImage = nullptr;
	}
}

void YCbCrSplitAndDumpSeparateChanlesTest()
{
	IJYCbCrImage888* image;
	IJRGBImage* yCompImage;
	IJRGBImage* bCompImage;
	IJRGBImage* rCompImage;
	IJResult result = IJResult::UnknownResult;

	{
		dbg__profileBlock2("Loading image");
		image = new IJYCbCrImage888("output/ybr_bar_skybox.tga");
		if (image == nullptr)
		{
			DBG_REPORT_ERROR("YCbCr image contruction error");
			return;
		}

		result = image->Load();
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Loading YCbCr image failed: %d", static_cast<int>(result));
			return;
		}
	}

	{
		dbg__profileBlock2("Spliting the ybr image to three rgb images");
		
		yCompImage = new IJRGBImage();
		assert(yCompImage);
		
		bCompImage = new IJRGBImage();
		assert(bCompImage);

		rCompImage = new IJRGBImage();
		assert(rCompImage);

		result = IJImageTranslator::YBRToRGBCompSlit(image, yCompImage, bCompImage, rCompImage);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("YBR split error %d", static_cast<int>(result));
		}

		yCompImage->Save("output/ybr_bar_skybox_Y.tga");
		bCompImage->Save("output/ybr_bar_skybox_B.tga");
		rCompImage->Save("output/ybr_bar_skybox_R.tga");
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
	
	//YCbCrSplitAndDumpSeparateChanlesTest();

	YBRToRGBTranslate();

	return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
