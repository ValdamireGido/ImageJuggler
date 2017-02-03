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

int main(int argc, char** argv)
{
	RGBLoadUnload();
	RGBToYCbCrTranslate();

	return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
//
//#include <CppUnitTest.h>
//namespace ProgramTestingPart
//{
//	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
//	namespace IJRGBImageTest
//	{
//		IJResult LoadRGBImage(IJRGBImage* image, const std::string& fileName)
//		{
//			image = new IJRGBImage(fileName);
//			if (!image)
//			{
//				return IJResult::MemoryAllocationError;
//			}
//
//			return image->Load();
//		}
//
//		IJResult SaveRGBImage(IJRGBImage* image)
//		{
//			if (!image)
//			{
//				return IJResult::BadMemoryPointer;
//			}
//
//			return image->Save();
//		}
//
//		IJResult UnloadRGBImage(IJRGBImage* image)
//		{
//			if (!image)
//			{
//				return IJResult::BadMemoryPointer;
//			}
//
//			delete image;
//			return IJResult::Success;
//		}
//
//
//		TEST_CLASS(IJRGBImageTestClass)
//		{
//		public:
//			TEST_METHOD(TestIJRGBImage)
//			{
//				uint32_t expectedResult		= static_cast<uint32_t>(IJResult::Success);
//
//				uint32_t loadActualResult	= static_cast<uint32_t>(LoadRGBImage(m_image, m_fileName));
//				uint32_t saveActualResult	= static_cast<uint32_t>(SaveRGBImage(m_image));
//				uint32_t unloadActualResult	= static_cast<uint32_t>(UnloadRGBImage(m_image));
//
//				Assert::AreEqual(expectedResult, loadActualResult,		L"Load RGB Image test");
//				Assert::AreEqual(expectedResult, saveActualResult, 		L"Save RGB Image test");
//				Assert::AreEqual(expectedResult, unloadActualResult,	L"Unload RGB Image test");
//			}
//
//		private:
//			IJRGBImage* m_image		= nullptr;
//			std::string m_fileName	= "input/bar_skybox.tga";
//		};
//	}
//}