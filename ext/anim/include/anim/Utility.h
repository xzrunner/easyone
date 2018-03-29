#pragma once

#include "anim/typedef.h"

#include <vector>

namespace anim
{

class Utility
{
public:
	static int GetMaxFrame(const std::vector<LayerPtr>& layers);

}; // Utility

}