#pragma once

#include "frame/config.h"

#ifdef MODULE_SCALE9

#include <ee0/GameObj.h>
#include <ee0/EditOP.h>

#include <SM_Vector.h>

ECS_WORLD_DECL

namespace eone
{

class WxPreviewPanel;

namespace scale9
{

class ResizeScale9OP : public ee0::EditOP
{
public:
	ResizeScale9OP(const std::shared_ptr<pt0::Camera>& camera,
		WxPreviewPanel* stage, ECS_WORLD_PARAM const ee0::GameObj& obj);

	virtual bool OnMouseLeftDown(int x, int y) override;
	virtual bool OnMouseLeftUp(int x, int y) override;
	virtual bool OnMouseDrag(int x, int y) override;

	virtual bool OnDraw(const ur::Device& dev, ur::Context& ctx) const override;

private:
	enum Status
	{
		STAT_NULL = 0,
		STAT_LEFT_LOW,
		STAT_RIGHT_LOW,
		STAT_LEFT_UP,
		STAT_RIGHT_UP
	};

private:
	WxPreviewPanel*  m_stage;

	ECS_WORLD_SELF_DEF
	ee0::GameObj m_obj;

	sm::vec2 m_first_pos;

	Status m_stat;

}; // ResizeScale9OP

}
}

#endif // MODULE_SCALE9