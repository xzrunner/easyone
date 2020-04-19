#include "scale9/ResizeScale9OP.h"

#ifdef MODULE_SCALE9

#include "frame/WxPreviewPanel.h"
#include "frame/WxPreviewCanvas.h"

#include <ee0/CameraHelper.h>
#include <ee0/SubjectMgr.h>
#include <ee2/CamControlOP.h>

#include <guard/check.h>
#include <SM_Calc.h>
#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>
#include <node2/CompScale9.h>
#include <node2/CompTransform.h>
#else
#include <entity0/World.h>
#include <entity2/CompBoundingBox.h>
#include <entity2/CompScale9.h>
#endif // GAME_OBJ_ECS
#include <tessellation/Painter.h>
#include <painting2/RenderSystem.h>

namespace
{

const int REGION = 10;

}

namespace eone
{
namespace scale9
{

ResizeScale9OP::ResizeScale9OP(const std::shared_ptr<pt0::Camera>& camera,
	                           WxPreviewPanel* stage, ECS_WORLD_PARAM
	                           const ee0::GameObj& obj)
	: ee0::EditOP(camera)
	, m_stage(stage)
	ECS_WORLD_SELF_ASSIGN
	, m_obj(obj)
	, m_stat(STAT_NULL)
{
	auto cam = std::dynamic_pointer_cast<WxPreviewCanvas>(stage->GetImpl().GetCanvas())->GetCamera();
	GD_ASSERT(cam, "null cam");

	SetPrevEditOP(std::make_shared<ee2::CamControlOP>(camera, stage->GetSubjectMgr()));
}

bool ResizeScale9OP::OnMouseLeftDown(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftDown(x, y)) {
		return true;
	}

	auto cam = std::dynamic_pointer_cast<WxPreviewCanvas>(m_stage->GetImpl().GetCanvas())->GetCamera();
	GD_ASSERT(cam, "null cam");
	m_first_pos = ee0::CameraHelper::TransPosScreenToProject(*cam, x, y);

#ifndef GAME_OBJ_ECS
	auto& sz = m_obj->GetUniqueComp<n2::CompBoundingBox>().GetSize();
#else
	auto& sz = m_world.GetComponent<e2::CompBoundingBox>(m_obj).rect;
#endif // GAME_OBJ_ECS
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
#ifndef GAME_OBJ_ECS
	auto& cscale9 = m_obj->GetSharedComp<n2::CompScale9>();
	cscale9.SetSize(w, h);

	auto& cbb = m_obj->GetUniqueComp<n2::CompBoundingBox>();
	cbb.SetSize(*m_obj, sm::rect(w, h));
#else
	auto& cscale9 = m_world.GetComponent<e2::CompScale9>(m_obj);
	cscale9.SetSize(m_world, w, h);

	auto& cbb = m_world.GetComponent<e2::CompBoundingBox>(m_obj);
	cbb.rect = sm::rect(w, h);
#endif // GAME_OBJ_ECS

	m_stage->GetSubjectMgr()->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);

	return false;
}

bool ResizeScale9OP::OnDraw(const ur2::Device& dev, ur2::Context& ctx) const
{
	if (ee0::EditOP::OnDraw()) {
		return true;
	}

#ifndef GAME_OBJ_ECS
	auto& cscale9 = m_obj->GetSharedComp<n2::CompScale9>();
	if (cscale9.GetType() == n2::CompScale9::S9_NULL) {
		return false;
	}

	auto& sz = m_obj->GetUniqueComp<n2::CompBoundingBox>().GetSize();
#else
	auto& cscale9 = m_world.GetComponent<e2::CompScale9>(m_obj);
	if (cscale9.type == e2::CompScale9::S9_NULL) {
		return false;
	}

	auto& sz = m_world.GetComponent<e2::CompBoundingBox>(m_obj).rect;
#endif // GAME_OBJ_ECS
	const float hw = sz.Width() * 0.5f,
		        hh = sz.Height() * 0.5f;
	const float r = REGION;

	tess::Painter pt;
	pt.AddRect(sm::vec2(-hw, -hh), sm::vec2(-hw + r, -hh + r), 0xffffffff);
	pt.AddRect(sm::vec2( hw, -hh), sm::vec2( hw + r, -hh + r), 0xffffffff);
	pt.AddRect(sm::vec2( hw,  hh), sm::vec2( hw + r,  hh + r), 0xffffffff);
	pt.AddRect(sm::vec2(-hw,  hh), sm::vec2(-hw + r,  hh + r), 0xffffffff);
	pt2::RenderSystem::DrawPainter(pt);

	return false;
}

}
}

#endif // MODULE_SCALE9