#pragma once

#include <cstdint>

namespace eone
{

static const uint32_t SHOW_LIBRARY   = 0x0001;
static const uint32_t SHOW_RECORD    = 0x0002;
static const uint32_t SHOW_STAGE     = 0x0004;
static const uint32_t SHOW_STAGE_EXT = 0x0008;
static const uint32_t SHOW_PREVIEW   = 0x0010;
static const uint32_t SHOW_WORLD     = 0x0020;
static const uint32_t SHOW_DETAIL    = 0x0040;
static const uint32_t SHOW_TOOLBAR   = 0x0080;
static const uint32_t SHOW_SCRIPT    = 0x0100;

static const uint32_t TOOLBAR_LFET   = 0x0200;
static const uint32_t STAGE_EXT_LFET = 0x0400;

//static const uint32_t MID_SHADER_SPLIT = 0x0200;

}