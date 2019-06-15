#pragma once

#include "frame/config.h"

#ifdef MODULE_SCALE9

#include <SM_Vector.h>

namespace eone
{
namespace scale9
{

class ComposeGrids
{
public:
	static void Draw();

	static void Query(const sm::vec2& pos, int* col, int* row);

	static sm::vec2 GetGridCenter(int col, int row);

}; // ComposeGrids

}
}

#endif // MODULE_SCALE9