#include "scale9/ResizeScale9OP.h"

#include "frame/WxPreviewPanel.h"
#include "frame/WxPreviewCanvas.h"

#include <ee0/CameraHelper.h>
#include <ee0/SubjectMgr.h>
#include <ee2/CamControlOP.h>

#include <guard/check.h>
#include <SM_Calc.h>
#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>
#include <node2/CompScale9.h>
#include <node2/CompTransform.h>
#include <painting2/PrimitiveDraw.h>

namespace
{

const int REGION = 10;

}

namespace eone
{
namespace scale9
{

ResizeScale9OP::ResizeScale9OP(WxPreviewPanel* stage, const ee0::GameObj& obj)
	: ee0::EditOP()
	, m_stage(stage)
	, m_obj(obj)
	, m_stat(STAT_NULL)
{
	auto cam = std::dynamic_pointer_cast<WxPreviewCanvas>(stage->GetImpl().GetCanvas())->GetCamera();
	GD_ASSERT(cam, "null cam");

	SetPrevEditOP(std::make_shared<ee2::CamControlOP>(*cam, stage->GetSubjectMgr()));
}

bool ResizeScale9OP::OnMouseLeftDown(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftDown(x, y)) {
		return true;
	}

	auto cam = std::dynamic_pointer_cast<WxPreviewCanvas>(m_stage->GetImpl().GetCanvas())->GetCamera();
	GD_ASSERT(cam, "null cam");
	m_first_pos = ee0::CameraHelper::TransPosScreenToProject(*cam, x, y);

	auto& cbb = m_obj->GetUniqueComp<n2::CompBoundingBox>();
	auto& sz = cbb.GetSize();
	const float hw = sz.Width() * 0.5f,
		        hh = sz.Height() * 0.5f;
	if (sm::is_point_in_rect(m_first_pos, sm::rect(sm::vec2(-hw, -hh), REGION, REGION))) {
		m_stat = STAT_LEFT_LOW;
	} else if (sm::is_point_in_rect(m_first_pos, sm::rect(sm::vec2(hw, -hh), REGION, REGION))) {
		m_stat = STAT_RIGHT_LOW;
	} else if (sm::is_point_in_rect(m_first_pos, sm::rect(sm::vec2(hw, hh), REGION, REGION))) {
		m_stat = STAT_RIGHT_UP;
	} else if (sm::is_point_in_rect(m_first_pos, sm::rect(sm::vec2(-hw, hh), REGION, REGION))) {
		m_stat = STAT_LEFT_UP;
	}

	return false;
}

bool ResizeScale9OP::OnMouseLeftUp(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftUp(x, y)) {
		return true;
	}

	//if (m_stat != STAT_NULL)
	//{
	//	auto pos = ee0::CameraHelper::TransPosScreenToProject(*m_camera, x, y);
	//	const sm::vec2 src(fabs(m_first_pos.x) * 2, fabs(m_first_pos.y) * 2),
	//		           dst(fabs(pos.x) * 2, fabs(pos.y) * 2);
	//	auto op = std::make_shared<ResizeAtomicOP>(m_sym, src, dst);
	//	ee::EditAddRecordSJ::Instance()->Add(op);
	//}

	m_stat = STAT_NULL;

	return false;
}

bool ResizeScale9OP::OnMouseDrag(int x, int y)
{
	if (ee0::EditOP::OnMouseDrag(x, y)) {
		return true;
	}

	if (m_stat == STAT_NULL) {
		return false;
	}

	auto cam = std::dynamic_pointer_cast<WxPreviewCanvas>(m_stage->GetImpl().GetCanvas())->GetCamera();
	GD_ASSERT(cam, "null cam");
	auto pos = ee0::CameraHelper::TransPosScreenToProject(*cam, x, y);

	float w = fabs(pos.x) * 2,
		  h = fabs(pos.y) * 2;
	auto& cscale9 = m_obj->GetSharedComp<n2::CompScale9>();
	cscale9.SetSize(w, h);

	auto& cbb = m_obj->GetUniqueComp<n2::CompBoundingBox>();
	cbb.SetSize(*m_obj, sm::rect(w, h));

	m_stage->GetSubjectMgr()->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);

	return false;
}

bool ResizeScale9OP::OnDraw() const
{
	if (ee0::EditOP::OnDraw()) {
		return true;
	}

	auto& cscale9 = m_obj->GetSharedComp<n2::CompScale9>();
	if (cscale9.GetType() == n2::CompScale9::S9_NULL) {
		return false;
	}

	auto& cbb = m_obj->GetUniqueComp<n2::CompBoundingBox>();
	auto& sz = cbb.GetSize();
	const float hw = sz.Width() * 0.5f,
		        hh = sz.Height() * 0.5f;
	const float r = REGION;

	pt2::PrimitiveDraw::Rect(nullptr, sm::vec2(-hw, -hh), r, r, false);
	pt2::PrimitiveDraw::Rect(nullptr, sm::vec2( hw, -hh), r, r, false);
	pt2::PrimitiveDraw::Rect(nullptr, sm::vec2( hw,  hh), r, r, false);
	pt2::PrimitiveDraw::Rect(nullptr, sm::vec2(-hw,  hh), r, r, false);

	return false;
}

}
}