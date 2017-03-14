#pragma once 

#include "IJImage.h"
#include <array>

///** IJYCbCrCompsContainer - Image Juggler YCbCr components container
//	Container for YCbCr image format pixels. It has separated comps 
//	for Y chanel and BR comps. 
//	Y - 8 bits
//	B - 4 bits
//	R - 4 bits
//
//	B and R are stored in one uint8_t field.
//*/
//struct IJYCbCrCompsContainer
//{
//	void push_back(uint8_t _y_comp, uint8_t _br_comp);
//	void push_y	  (uint8_t _y_comp);
//	void push_br  (uint8_t _br_comp);
//
//	uint8_t  get_y  (size_t idx);
//	uint8_t  get_br (size_t idx);
//	uint16_t get_ybr(size_t idx);
//
//	std::vector<uint8_t> y_data;
//	std::vector<uint8_t> br_data;
//};
//
//// Image definition
//
//class IJYCbCrImage422
//	: public IJImageInterface<uint8_t>
//{
//public:
//	using PixelData_t	= IJYCbCrCompsContainer;
//
//public:
//	IJYCbCrImage422();
//	IJYCbCrImage422(const std::string& fileName);
//	IJYCbCrImage422(const std::vector<uint8_t>& y_comps, 
//					const std::vector<uint8_t>& br_comps);
//	~IJYCbCrImage422();
//
//	IJResult Load(std::istream& iStream) override;
//	IJResult Save(std::ostream& oStream) override;
//	
//	IJResult Load(const std::vector<uint8_t>& y_comps, 
//				  const std::vector<uint8_t>& br_comps);
//	IJResult Save(		std::vector<uint8_t>& y_comps, 
//						std::vector<uint8_t>& br_comps);
//
//	using IJImageInterface<uint8_t>::Load;
//	using IJImageInterface<uint8_t>::Save;
//};
