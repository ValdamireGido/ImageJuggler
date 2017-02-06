#include "stdafx.h"
#include "CppUnitTest.h"

#include "Image/IJRGBImage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestImageJuggler
{
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
			TEST_METHOD(TestIJRGBImageLoad)
			{
				uint32_t expectedResult		= static_cast<uint32_t>(IJResult::Success);
				uint32_t loadActualResult	= static_cast<uint32_t>(LoadRGBImage(m_image, m_fileName));

				Assert::AreEqual(expectedResult, loadActualResult, L"Load RGB Image test");
			}

			TEST_METHOD(TestIJRGBImageSave)
			{
				uint32_t expectedResult		= static_cast<uint32_t>(IJResult::Success);
				uint32_t saveActualResult	= static_cast<uint32_t>(SaveRGBImage(m_image));
				Assert::AreEqual(expectedResult, saveActualResult, L"Unload RGB Image test");
			}

			TEST_METHOD(TestIJRGBImageUnload)
			{
				uint32_t expectedResult		= static_cast<uint32_t>(IJResult::Success);
				uint32_t unloadActualResult	= static_cast<uint32_t>(UnloadRGBImage(m_image));
				Assert::AreEqual(expectedResult, unloadActualResult, L"Unload RGB Image test");
			}

		private:
			IJRGBImage* m_image		= nullptr;
			std::string m_fileName	= "input/bar_skybox.tga";
		};
	}
}
