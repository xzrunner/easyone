#pragma once

namespace eone
{

enum NodeType
{
	NODE_UNKNOWN = 0,

	NODE_IMAGE = 1,
	NODE_TEXT,
	NODE_MASK,
	NODE_MESH,
	NODE_SCALE9,
	NODE_SCENE2D,
	NODE_SCENE3D,

	NODE_SPRITE2,
};

}