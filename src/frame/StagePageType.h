#pragma once

#include <string>

namespace eone
{

enum StagePageType
{
	PAGE_INVALID = 0,

	PAGE_SCENE2D,
	PAGE_SCENE3D,

	PAGE_SCALE9,
	PAGE_MASK,
	PAGE_MESH,
	PAGE_ANIM,
	PAGE_PARTICLE3D,
	PAGE_SHAPE2D,

	PAGE_MODEL,
	PAGE_ANIM3,
	PAGE_SHADER_GRAPH,
	PAGE_PROTOTYPING,

	PAGE_SCRIPT,
	PAGE_BLUEPRINT,

	PAGE_QUAKE,
};

inline
std::string GetPageName(int type)
{
	switch (type)
	{
	case PAGE_SCENE2D:
		return "Scene2D";
	case PAGE_SCENE3D:
		return "Scene3D";

	case PAGE_SCALE9:
		return "Scale9";
	case PAGE_MASK:
		return "Mask";
	case PAGE_MESH:
		return "Mesh";
	case PAGE_ANIM:
		return "Anim";
	case PAGE_PARTICLE3D:
		return "Particle3D";
	case PAGE_SHAPE2D:
		return "Shape2D";

	case PAGE_MODEL:
		return "Model";
	case PAGE_ANIM3:
		return "Anim3";
	case PAGE_SHADER_GRAPH:
		return "ShaderGraph";
	case PAGE_PROTOTYPING:
		return "Prototyping";

	case PAGE_SCRIPT:
		return "Script";
	case PAGE_BLUEPRINT:
		return "Blueprint";

	case PAGE_QUAKE:
		return "Quake";

	default:
		return "";
	}
}

}