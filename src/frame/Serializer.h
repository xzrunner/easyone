#pragma once

#include <string>

namespace ee0 { class WxStagePage; }

namespace eone
{

class Serializer
{
public:
	static void StoreToFile(const ee0::WxStagePage& page, const std::string& filepath);
	static void LoadFromFile(ee0::WxStagePage& page, const std::string& filepath);

}; // Serializer

}