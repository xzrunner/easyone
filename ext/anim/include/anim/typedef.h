#pragma once

#include <memory>

namespace anim
{

class KeyFrame;
using KeyFramePtr = std::unique_ptr<KeyFrame>;

class Layer;
using LayerPtr = std::unique_ptr<Layer>;

}