#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <assert.h>

#include "ImageJuggler/Image/IJRGBImage.h"

int main(int argc, char** argv)
{
	//bar_skybox.tga
	std::string inputImage("input/Atlas_Cars.tga");
	IJRGBImage imageAtlasCars;
	IJResult result = imageAtlasCars.Load(inputImage);
	if (result != IJResult::Success)
	{
		return EXIT_FAILURE;
	}

	std::vector<uint8_t> rawImage;
	if (imageAtlasCars.Save(rawImage) != IJResult::Success)
	{
		rawImage.clear();
	}

	if (rawImage.empty())
	{
		return EXIT_FAILURE;
	}

	std::string outputImage = std::string("output/imageOutput.tga");
	IJRGBImage newImage(outputImage);
	newImage.Load(rawImage);
	if (newImage.Save() != IJResult::Success)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

#include <CppUnitTest.h>
namespace ProgramTestingPart
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	namespace IJRGBImageTest
	{
		IJResult LoadRGBImage(IJRGBImage* image, const std::string& fileName)
		{
			image = new IJRGBImage(fileName);
			if (!image)
			{
				return IJResult::MemoryAllocationError;
			}

			return image->Load();
		}

		IJResult SaveRGBImage(IJRGBImage* image)
		{
			if (!image)
			{
				return IJResult::BadMemoryPointer;
			}

			return image->Save();
		}

		IJResult UnloadRGBImage(IJRGBImage* image)
		{
			if (!image)
			{
				return IJResult::BadMemoryPointer;
			}

			delete image;
			return IJResult::Success;
		}


		TEST_CLASS(IJRGBImageTestClass)
		{
		public:
			TEST_METHOD(LoadIJRGBImageTestMethod)
			{
				uint32_t expectedResult = static_cast<uint32_t>(IJResult::Success);
				uint32_t actualResult	= static_cast<uint32_t>(LoadRGBImage(m_image, m_fileName));
				Assert::AreEqual(expectedResult, actualResult);
			}

			TEST_METHOD(SaveIJRGBImageTestMethod)
			{
				uint32_t expectedResult = static_cast<uint32_t>(IJResult::Success);
				uint32_t actualResult	= static_cast<uint32_t>(SaveRGBImage(m_image));
				Assert::AreEqual(expectedResult, actualResult);
			}

			TEST_METHOD(UnloadIJRGBImageTestMethod)
			{
				uint32_t expectedResult = static_cast<uint32_t>(IJResult::Success);
				uint32_t actualResult	= static_cast<uint32_t>(UnloadRGBImage(m_image));
				Assert::AreEqual(expectedResult, actualResult);
			}

		private:
			IJRGBImage* m_image		= nullptr;
			std::string m_fileName	= "input/bar_skybox.tga";
		};
	}
}