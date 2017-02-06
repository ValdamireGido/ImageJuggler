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
		dbg__profileBlock("Contructing IJRGBImage");
		image = new IJRGBImage("input/alp_tile_Cable_Bridge_df.tga");
	}

	IJResult result = IJResult::Success;
	
	{
		dbg__profileBlock("Loading RGB Image");
		result = image->Load();
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Load error: %d", static_cast<int>(result));
		}
	}
	
	{
		dbg__profileBlock("Save RGB image to vector<uint8_t>");
		std::vector<uint8_t> rawImage;
		result = image->Save(rawImage);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Save to raw image error: %d", static_cast<int>(result));
		}
	}

	{
		dbg__profileBlock("RGB image save to file");
		result = image->Save("output/alp_tile_Cable_Bridge_df.tga");
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Save to file error: %d", static_cast<uint32_t>(result));
		}
	}

	{
		dbg__profileBlock("Destructing RGB image");
		delete image;
		image = nullptr;
	}
}

void RGBToYCbCrTranslate()
{
	IJRGBImage* rgbImage = nullptr;
	IJYCbCrImage444* ybrImage = nullptr;
	IJResult result = IJResult::Success;

	{
		dbg__profileBlock("Constructing RGB image");
		rgbImage = new IJRGBImage("input/alp_tile_Cable_Bridge_df.tga");
		if (rgbImage == nullptr)
		{
			DBG_REPORT_ERROR("RGB imabe contruct failed");
		}
	}

	{
		dbg__profileBlock("Loading RGB image");
		result = rgbImage->Load();
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("RGB image load failed: %d", static_cast<int>(result));
		}
	}

	{
		dbg__profileBlock("Constructing IJYCbCr444 image");
		ybrImage = new IJYCbCrImage444();
		if (ybrImage == nullptr)
		{
			DBG_REPORT_ERROR("YCbCr image construct failed");
		}
	}

	{
		dbg__profileBlock("Translation RGB -> YCbCr444");
		result = IJImageTranslator::RGBToYCbCr444(rgbImage, ybrImage);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("RGB -> YCbCr444 translation failed: %d", static_cast<int>(result));
		}
	}

	{
		dbg__profileBlock("Save YCbCr to file");
		result = ybrImage->Save("output/ybr_alp_tile_Cable_Bridge_df.tga");
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("YCbCr image save failed: %d", static_cast<int>(result));
		}
	}

	{
		dbg__profileBlock("Destructing YCbCr image");
		delete ybrImage;
		ybrImage = nullptr;
	}

	{
		dbg__profileBlock("Destructing RGB image");
		delete rgbImage;
		rgbImage = nullptr;
	}
}

void YCbCrSplitAndDumpSeparateChanlesTest()
{
	IJYCbCrImage444* image;
	IJResult result = IJResult::Success;

	{
		dbg__profileBlock("Constructing YCbCrImage");
		image = new IJYCbCrImage444("output/ybr_alp_tile_Cable_Bridge_df.tga");
		if (image == nullptr)
		{
			DBG_REPORT_ERROR("YCbCr image contruction error");
		}
	}

	{
		dbg__profileBlock("Loading image");
		result = image->Load();
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Loading YCbCr image failed: %d", static_cast<int>(result));
		}
	}

	{
		dbg__profileBlock("Spliting ycbcr image");

		std::vector<uint8_t> yComp;
		std::vector<uint8_t> bComp;
		std::vector<uint8_t> rComp;

		std::vector<uint8_t> bCompResult;
		std::vector<uint8_t> rCompResult;

		result = IJImageTranslator::YCbCrCompSplit(image, yComp, bComp, rComp);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Error spliting ycbcr image by comps");
		}

		{
			dbg__profileBlock("Setting rgb raw images for componenets");
			yComp.insert(yComp.end(), yComp.begin(), yComp.end());
			yComp.insert(yComp.end(), yComp.begin(), yComp.end());

			for (size_t i = 0; i < bComp.size(); i++)
			{
				std::array<uint8_t, 3> pixel = IJImageTranslator::TranslateYBRPixelToRGB({yComp[i], bComp[i], rComp[i]});

			}
		}

		// spliting ycbcr image by 3 different images
		IJRGBImage* yImage = new IJRGBImage(yComp);
		IJRGBImage* bImage = new IJRGBImage(bComp);
		IJRGBImage* rImage = new IJRGBImage(rComp);
	}
}

int main(int argc, char** argv)
{
	//RGBLoadUnload();
	//RGBToYCbCrTranslate();
	YCbCrSplitAndDumpSeparateChanlesTest();

	return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
