#include "model/WxPreviewCanvas.h"
#include "model/WxPreviewPanel.h"
#include "model/RenderSystem.h"

#include <ee0/SubjectMgr.h>

#include <node0/SceneNode.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>

namespace eone
{
namespace model
{

WxPreviewCanvas::WxPreviewCanvas(WxPreviewPanel* stage, ECS_WORLD_PARAM
	                             const ee0::RenderContext& rc, const ee0::GameObj& obj)
	: ee3::WxStageCanvas(stage, &rc, nullptr, false)
	, m_obj(obj)
{
	RegisterMsg(*stage->GetSubjectMgr());
}

void WxPreviewCanvas::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_SET_CANVAS_DIRTY:
		SetDirty();
		break;
	}
}

void WxPreviewCanvas::DrawBackground() const
{
}

void WxPreviewCanvas::DrawForeground() const
{
	if (!m_obj->HasSharedComp<n3::CompModel>()) {
		return;
	}

	n3::RenderParams params;
	params.mt = m_camera->GetModelViewMat();
	params.type = n3::RenderParams::DRAW_MESH;

	auto& cmodel     = m_obj->GetUniqueComp<n3::CompModelInst>();
	auto& model_inst = cmodel.GetModel();
	if (!model_inst) {
		return;
	}

	RenderSystem::Instance()->DrawModel(*model_inst, params);
}

void WxPreviewCanvas::RegisterMsg(ee0::SubjectMgr& sub_mgr)
{
	sub_mgr.RegisterObserver(ee0::MSG_SET_CANVAS_DIRTY, this);
}

}
}