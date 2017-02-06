#include "IJYCbCrImage422.h"
#include <assert.h>
#include <fstream>
#include <sstream>

// IJYCbCrCompsContainer Realization

void IJYCbCrCompsContainer::push_back(uint8_t _y_comp, uint8_t _br_comp)
{
	y_data.push_back(_y_comp);
	br_data.push_back(_br_comp);
}

void IJYCbCrCompsContainer::push_y(uint8_t _y_comp)
{
	y_data.push_back(_y_comp);
}

void IJYCbCrCompsContainer::push_br(uint8_t _br_comp)
{
	br_data.push_back(_br_comp);
}

uint8_t IJYCbCrCompsContainer::get_y(size_t idx)
{
	assert(idx < y_data.size());
	return y_data[idx];
}

uint8_t IJYCbCrCompsContainer::get_br(size_t idx)
{
	assert(idx < br_data.size());
	return br_data[idx];
}

uint16_t IJYCbCrCompsContainer::get_ybr(size_t idx)
{
	assert(false);
	return uint16_t(0);
}

// Image Realization

IJYCbCrImage422::IJYCbCrImage422()
	: IJImage<uint8_t>(IJImageType::YCbCr422)
{}

IJYCbCrImage422::IJYCbCrImage422(const std::string& fileName)
	: IJImage<uint8_t>(fileName, IJImageType::YCbCr422)
{}

IJYCbCrImage422::IJYCbCrImage422(const std::vector<uint8_t>& y_comps, 
								 const std::vector<uint8_t>& br_comps)
	: IJImage<uint8_t>(IJImageType::YCbCr422)
{
	SetSize(y_comps.size() + br_comps.size());
}

IJYCbCrImage422::~IJYCbCrImage422()
{}

IJResult IJYCbCrImage422::Load(std::istream& iStream)
{
	assert(false);
	return IJResult::Success;
}

IJResult IJYCbCrImage422::Save(std::ostream& oStream)
{
	assert(false);
	return IJResult::Success;
}
