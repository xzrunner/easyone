#include "model/WxStageCanvas.h"
#include "model/RenderSystem.h"
#include "model/WxStagePage.h"

#include "frame/WxStagePage.h"

#include <ee0/WxStagePage.h>
#include <ee0/SubjectMgr.h>

#include <model/Model.h>
#include <node0/SceneNode.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>

namespace eone
{
namespace model
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
	                         const ee0::RenderContext& rc)
	: WxStageCanvas3D(stage, rc, true)
	, m_obj(stage->GetEditedObj())
{
}

void WxStageCanvas::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	WxStageCanvas3D::OnNotify(msg, variants);

	switch (msg)
	{
	case ee0::MSG_SET_CANVAS_DIRTY:
		static_cast<model::WxStagePage*>(m_stage)->GetPreviewSubMgr()->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
		break;
	}
}

void WxStageCanvas::DrawBackground() const
{
}

void WxStageCanvas::DrawForeground() const
{
	if (!m_obj->HasSharedComp<n3::CompModel>()) {
		return;
	}

	n3::RenderParams params;
	params.mt   = m_camera->GetModelViewMat();
	params.type = n3::RenderParams::DRAW_MESH;

	auto& cmodel = m_obj->GetUniqueComp<n3::CompModelInst>();
	auto& model_inst = cmodel.GetModel();
	if (!model_inst) {
		return;
	}

	auto& model = model_inst->GetModel();
	auto& ext = model->ext;
	if (ext->Type() == ::model::EXT_SKELETAL) {
		RenderSystem::Instance()->DrawModel(*model_inst, params, true);
	} else {
		n3::RenderSystem::Draw(m_obj, params);
	}
}

}
}