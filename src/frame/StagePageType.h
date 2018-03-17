#pragma once

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

	PAGE_SCRIPT,
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

	case PAGE_SCRIPT:
		return "Script";

	default:
		return "";
	}
}

}