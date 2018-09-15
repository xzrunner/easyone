#include "anim3/WxStageCanvas.h"
#include "anim3/WxStagePage.h"

#include "frame/WxStagePage.h"
#include "model/RenderSystem.h"

#include <ee0/WxStagePage.h>
#include <ee0/SubjectMgr.h>

#include <model/Model.h>
#include <node0/SceneNode.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <node3/RenderSystem.h>

namespace eone
{
namespace anim3
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
	                         const ee0::RenderContext& rc)
	: WxStageCanvas3D(stage, rc, true)
	, m_obj(stage->GetEditedObj())
{
}

void WxStageCanvas::DrawBackground() const
{
}

void WxStageCanvas::DrawForeground() const
{
	if (!m_obj->HasSharedComp<n3::CompModel>()) {
		return;
	}

	pt3::RenderParams params;
	params.mt   = m_camera->GetViewMat();
	params.type = pt3::RenderParams::DRAW_MESH;

	auto& cmodel = m_obj->GetUniqueComp<n3::CompModelInst>();
	auto& model_inst = cmodel.GetModel();
	if (!model_inst) {
		return;
	}

	auto& model = model_inst->GetModel();
	auto& ext = model->ext;
	if (ext->Type() == ::model::EXT_SKELETAL) {
		model::RenderSystem::Instance()->DrawModel(*model_inst, params, true);
	} else {
		n3::RenderSystem::Draw(m_obj, params);
	}
}

}
}