#pragma once

#include <ee0/EditOP.h>

#include <SM_Vector.h>
#include <node0/typedef.h>

namespace eone
{

class WxPreviewPanel;

namespace scale9
{

class ResizeScale9OP : public ee0::EditOP
{
public:
	ResizeScale9OP(WxPreviewPanel* stage, const n0::SceneNodePtr& node);

	virtual bool OnMouseLeftDown(int x, int y) override;
	virtual bool OnMouseLeftUp(int x, int y) override;
	virtual bool OnMouseDrag(int x, int y) override;

	virtual bool OnDraw() const override;

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
	n0::SceneNodePtr m_node;

	sm::vec2 m_first_pos;

	Status m_stat;

}; // ResizeScale9OP

}
}