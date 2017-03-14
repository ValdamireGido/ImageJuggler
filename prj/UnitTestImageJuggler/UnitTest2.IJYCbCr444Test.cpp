#include "stdafx.h"
#include "CppUnitTest.h"

#include "Image/IJYCbCrImage888.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestImageJuggler
{
	namespace IJYCbCrImageTest
	{
		IJResult LoadYBRImage(IJYCbCrImage888* image, const std::string& fileName)
		{
			if (!image)
			{
				image = new IJYCbCrImage888();
				Assert::AreNotEqual((intptr_t)nullptr, (intptr_t)image, L"Faile to create IJYCbCrImage444 instance");
				if (!image)
				{
					return IJResult::MemoryAllocationError;
				}
			}

			return image->Load(fileName);
		}

		IJResult SaveYBRImage(IJYCbCrImage888* image)
		{
			Assert::AreNotEqual((intptr_t)nullptr, (intptr_t)image, L"Invalid input pointer IJYCbCrImage444* image");
			if (!image)
			{
				return IJResult::BadMemoryPointer;
			}

			return image->Save();
		}

		IJResult UnloadYBRImage(IJYCbCrImage888* image)
		{
			Assert::AreNotEqual((intptr_t)nullptr, (intptr_t)image, L"Invalid input pointer IJYCbCrImage444* image");
			if (!image)
			{
				return IJResult::BadMemoryPointer;
			}

			delete image;
			image = nullptr;
			return IJResult::Success;
		}


		TEST_CLASS(IJYCbCrImage444TestClass)
		{
		public:
			//TEST_CLASS_INITIALIZE()

			TEST_METHOD(TestIJYCbCrImage444Load)
			{
				uint32_t expected	= static_cast<uint32_t>(IJResult::Success);
				uint32_t actual		= static_cast<uint32_t>(LoadYBRImage(m_image, m_fileName));
				Assert::AreEqual(expected, actual, L"Load YBR Image test fail");
			}

			TEST_METHOD(TestIJYCbCrImage444Save)
			{
				uint32_t expected	= static_cast<uint32_t>(IJResult::Success);
				uint32_t actual		= static_cast<uint32_t>(SaveYBRImage(m_image));
				Assert::AreEqual(expected, actual, L"Save YBR image test fail");
			}

			TEST_METHOD(TestIJYCbCrImage444Unload)
			{
				uint32_t expected	= static_cast<uint32_t>(IJResult::Success);
				uint32_t actual		= static_cast<uint32_t>(UnloadYBRImage(m_image));
				Assert::AreEqual(expected, actual, L"Unload YBR image test fail");
			}

		private:
			IJYCbCrImage888*	m_image		= nullptr;
			std::string			m_fileName	= "input/"; // NEED TO SET
		};
	}
}