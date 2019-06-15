#include "scale9/ComposeGrids.h"

#ifdef MODULE_SCALE9

#include <ee0/color_config.h>

#include <tessellation/Painter.h>
#include <painting2/RenderSystem.h>

namespace eone
{
namespace scale9
{

static const float EDGE = 100;
static const float X = EDGE * -1.5f;
static const float Y = EDGE * -1.5f;

void ComposeGrids::Draw()
{
	tess::Painter pt;
	for (size_t i = 0; i < 3; ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			sm::vec2 p0;
			p0.x = X + EDGE*i;
			p0.y = Y + EDGE*j;
			sm::vec2 p1(p0.x + EDGE, p0.y + EDGE);
			pt.AddRect(p0, p1, ee0::LIGHT_GREY.ToABGR());
		}
	}
	pt2::RenderSystem::DrawPainter(pt);
}

void ComposeGrids::Query(const sm::vec2& pos, int* col, int* row)
{
	if (pos.x < X || pos.x > X + EDGE * 3 ||
		pos.y < Y || pos.y > Y + EDGE * 3) {
		*col = -1;
		*row = -1;
		return;
	}

	*col = (int)((pos.x - X) / EDGE);
	*row = (int)((pos.y - Y) / EDGE);
}

sm::vec2 ComposeGrids::GetGridCenter(int col, int row)
{
	return sm::vec2(X + (col + 0.5f) * EDGE, Y + (row + 0.5f) * EDGE);
}

}
}

#endif // MODULE_SCALE9