#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <assert.h>
#include <sstream>

#include "IJImageTranslator.h"
#include "IJYUVPackedConverter.h"
#include "Image/IJRGBImage.h"
#include "Image/IJYCbCrImage888.h"
#include "Image/IJPackedColourImage.h"

#define	PROFILING_ENABLED 1
#include "Profiling.h"
std::ofstream s_profiling_output_file_stream;

#define profile_block(blockName) dbg__profileBlock2_file(blockName, s_profiling_output_file_stream)

#if defined(_DEBUG)
	#if defined(_WIN32) || defined(_WIN64)
		#define DBG_REPORT_ERROR(format, ...) _CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, __FUNCDNAME__, format, __VA_ARGS__)
		#define DBG_REPORT_WARN(format, ...) _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, __FUNCDNAME__, format, __VA_ARGS__)
	#elif defined(__APPLE__)
		#define DBG_REPORT_ERROR(format, ...)
		#define DBG_REPORT_WARN(format, ...)
	#endif
#else
	#define DBG_REPORT_ERROR(format, ...)
	#define DBG_REPORT_WARN(format, ...)
#endif

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

		result = IJImageTranslator::RGBToYCbCr888(rgbImage, ybrImage);
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

		result = IJImageTranslator::YCbCr888ToRGB(ybrImage, rgbImage);
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

void RGBToYUVPack()
{
	const std::string inputFileName  = "input/SourceG.tga";
	const std::string outputFileName = "output/SourceG.yuv.tga";

	IJResult result = IJResult::UnknownResult;
	IJPackedColourImage* packedImage = nullptr;
	IJRGBImage* rgbImage = nullptr;

	{
		profile_block("Loading rgb image");

		rgbImage = new IJRGBImage();
		assert(rgbImage);

		result = rgbImage->Load(inputFileName);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Can't load rgb image");
		}
	}


	{
		profile_block("Translating and packing the rgb image to packed YUV image");
		packedImage = new IJPackedColourImage();
		assert(packedImage);
		
		result = packedImage->PackRGBImage(rgbImage);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Can't pack the rgb to packed yuv image");
		}
	}


	{
		profile_block("Saving the result image");

		std::ofstream ofile(outputFileName, std::ios::out | std::ios::binary);
		result = packedImage->Save(ofile);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Error saving packed image %s", outputFileName.c_str());
		}

		ofile.close();

		delete rgbImage; 
		rgbImage = nullptr;

		delete packedImage;
		packedImage = nullptr;
	}
}

void YUVpackToRGB()
{
	const std::string inputFileName  = "output/SourceG.yuv.tga";
	const std::string outputFileName = "output/SourceG.yuv.rgb.tga";

	IJResult result = IJResult::UnknownResult;
	IJPackedColourImage* packedImage = nullptr;
	IJRGBImage* rgbImage = nullptr;

	{
		profile_block("Loading the packed image");

		packedImage = new IJPackedColourImage();
		assert(packedImage);

		std::ifstream ifile(inputFileName, std::ios::in | std::ios::binary);
		if (!ifile.is_open())
		{
			DBG_REPORT_ERROR("Cannot open input file");
		}

		result = packedImage->Load(ifile);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Cannot load yuv packed image");
		}
	}

	{
		profile_block("Unpacking the image");

		rgbImage = new IJRGBImage();
		assert(rgbImage);

#define TESTING_THE_NEW_GENERIC_ALGORITHM 1
#if TESTING_THE_NEW_GENERIC_ALGORITHM
		int error = IJYuvPackedConverter::unpack(packedImage, rgbImage);
		if (error)
		{
			DBG_REPORT_ERROR("error upacking the yuv packed image");
		}
#else 
		result = packedImage->UnpackRGBImage(rgbImage);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Cannot unpack the yuv image to rgb");
		}
#endif
	}

	{
		profile_block("Clean up");

		result = rgbImage->Save(outputFileName);
		if (result != IJResult::Success)
		{
			DBG_REPORT_ERROR("Cannot save rgb file");
		}

		delete rgbImage;
		rgbImage = nullptr;

		delete packedImage;
		packedImage = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	s_profiling_output_file_stream.open("dbgoutput/profiling_output", std::ios::out | std::ios::app);
	s_profiling_output_file_stream << std::endl << "Run on " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;

	RGBToYUVPack();
	YUVpackToRGB();

	return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
