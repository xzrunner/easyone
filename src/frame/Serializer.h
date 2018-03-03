#pragma once

#include <string>

namespace eone
{
	
class WxStagePage;

class Serializer
{
public:
	static void StoreToFile(const WxStagePage& page, const std::string& filepath);
	static void LoadFromFile(WxStagePage& page, const std::string& filepath);

}; // Serializer

}