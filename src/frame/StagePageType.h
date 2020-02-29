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
	PAGE_SHAPE3D,
    PAGE_CAD,

	PAGE_MODEL,
	PAGE_ANIM3,
	PAGE_SHADER_GRAPH,
	PAGE_PROTOTYPING,
    PAGE_RENDER_GRAPH,
    PAGE_PHYSICS3D,
    PAGE_RAY_GRAPH,
    PAGE_GUI_GRAPH,
    PAGE_PBR_GRAPH,
    PAGE_HDI_SOP,
    PAGE_HDI_VOP,
    PAGE_CITY_ENGINE,
    PAGE_TERRAIN,
    PAGE_GRASSHOPPER,
    PAGE_TOUCH_DESIGNER,
    PAGE_VISION_LAB,
    PAGE_SUBSTANCE_DESIGNER,

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
	case PAGE_SHAPE3D:
		return "Shape3D";
    case PAGE_CAD:
        return "CAD";

	case PAGE_MODEL:
		return "Model";
	case PAGE_ANIM3:
		return "Anim3";
	case PAGE_SHADER_GRAPH:
		return "ShaderGraph";
	case PAGE_PROTOTYPING:
		return "Prototyping";
    case PAGE_RENDER_GRAPH:
        return "RenderGraph";
    case PAGE_RAY_GRAPH:
        return "RayGraph";
    case PAGE_GUI_GRAPH:
        return "GuiGraph";
    case PAGE_PBR_GRAPH:
        return "PbrGraph";
    case PAGE_HDI_SOP:
        return "HdiSop";
    case PAGE_HDI_VOP:
        return "HdiVop";
    case PAGE_CITY_ENGINE:
        return "CityEngine";
    case PAGE_TERRAIN:
        return "Terrain";
    case PAGE_GRASSHOPPER:
        return "Grasshopper";
    case PAGE_TOUCH_DESIGNER:
        return "TouchDesigner";
    case PAGE_VISION_LAB:
        return "VisionGraph";
    case PAGE_SUBSTANCE_DESIGNER:
        return "SubstanceDesigner";

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