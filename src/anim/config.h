#pragma once

namespace eone
{
namespace anim
{

static const int FRAME_GRID_WIDTH = 12;
static const int FRAME_GRID_HEIGHT = FRAME_GRID_WIDTH * 2;
#ifdef ANIM_LONG_TIMELINE
static const int MAX_FRAME_COUNT = 4000;
#else
static const int MAX_FRAME_COUNT = 500;
#endif // ANIM_LONG_TIMELINE

static const wxColour LIGHT_GRAY = wxColour(229, 229, 229);
static const wxColour MEDIUM_GRAY = wxColour(180, 180, 180);
static const wxColour DARK_GRAY = wxColour(140, 140, 140);

static const wxColour LIGHT_BLUE = wxColour(125, 186, 247);
static const wxColour MEDIUM_BLUE = wxColour(117, 178, 239);
static const wxColour DARK_BLUE = wxColour(51, 153, 255);

static const wxColour LIGHT_RED = wxColour(255, 0, 0);
static const wxColour MEDIUM_RED = wxColour(255, 153, 153);
static const wxColour DARK_RED = wxColour(204, 0, 0);

static const wxColour TEXT_COLOR = wxColour(90, 80, 60);

static const wxColour CLASSIC_TWEEN_COLOR = wxColour(204, 204, 255);

}
}