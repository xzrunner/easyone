#include "anim3/WxStageCanvas.h"
#include "anim3/WxStagePage.h"

#include "frame/WxStagePage.h"

#include <ee0/WxStagePage.h>
#include <ee0/SubjectMgr.h>

#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <model/Model.h>
#include <node0/SceneNode.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <node3/RenderSystem.h>
#include <painting3/MaterialMgr.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>

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
	params.type = pt3::RenderParams::DRAW_MESH;

    pt0::RenderContext ctx;
    ctx.AddVar(
        pt3::MaterialMgr::PositionUniforms::light_pos.name,
        pt0::RenderVariant(sm::vec3(0, 2, -4))
    );
    auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
    assert(wc);
    ctx.AddVar(
        pt3::MaterialMgr::PosTransUniforms::view.name,
        pt0::RenderVariant(wc->GetViewMat())
    );
    ctx.AddVar(
        pt3::MaterialMgr::PosTransUniforms::projection.name,
        pt0::RenderVariant(wc->GetProjMat())
    );

	auto& cmodel_inst = m_obj->GetUniqueComp<n3::CompModelInst>();
	auto& model_inst = cmodel_inst.GetModel();
	if (!model_inst) {
		return;
	}

	auto& model = model_inst->GetModel();
	auto& ext = model->ext;
	if (ext->Type() == ::model::EXT_SKELETAL)
    {
        auto& cmodel = m_obj->GetSharedComp<n3::CompModel>();
        auto& mats = cmodel.GetAllMaterials();

        auto& rc = ur::Blackboard::Instance()->GetRenderContext();
        rc.SetPolygonMode(ur::POLYGON_LINE);
        pt3::RenderSystem::Instance()->DrawModel(*model_inst, mats, params, ctx);
        rc.SetPolygonMode(ur::POLYGON_FILL);
	}
    else
    {
		n3::RenderSystem::Draw(*m_obj, params, ctx);
	}
}

}
}