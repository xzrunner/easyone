#pragma once

#include "frame/config.h"

#ifdef MODULE_SHADERGRAPH

namespace eone
{
namespace sgraph
{

enum class ModelType
{
    UNKNOWN,
    SPRITE,
    PHONG,
    PBR,
    RAYMARCHING,
};

}
}

#endif // MODULE_SHADERGRAPH