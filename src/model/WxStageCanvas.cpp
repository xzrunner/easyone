#include "model/WxStageCanvas.h"
#include "model/WxStagePage.h"

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
#include <tessellation/Painter.h>
#include <painting2/RenderSystem.h>
#include <painting3/RenderSystem.h>
#include <painting3/MaterialMgr.h>


namespace eone
{
namespace model
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
	                         const ee0::RenderContext& rc)
	: WxStageCanvas3D(stage, rc, true)
	, m_obj(stage->GetEditedObj())
{
	InitGui();
}

void WxStageCanvas::DrawBackground() const
{
	// draw cross

	tess::Painter pt;

	auto cam_mat = m_camera->GetViewMat() * m_camera->GetProjectionMat();
	auto trans3d = [&](const sm::vec3& pos3)->sm::vec2 {
		return GetViewport().TransPosProj3ToProj2(pos3, cam_mat);
	};

	const float len = 1;
	pt.AddLine3D({ -len, 0, 0 }, { len, 0, 0 }, trans3d, 0xff0000ff, 2);
	pt.AddLine3D({ 0, -len, 0 }, { 0, len, 0 }, trans3d, 0xff00ff00, 2);
	pt.AddLine3D({ 0, 0, -len }, { 0, 0, len }, trans3d, 0xffff0000, 2);

	const float radius = 0.1f;
	pt.AddCircleFilled(trans3d(sm::vec3(len, 0, 0)), radius, 0xff0000ff);
	pt.AddCircleFilled(trans3d(sm::vec3(0, len, 0)), radius, 0xff00ff00);
	pt.AddCircleFilled(trans3d(sm::vec3(0, 0, len)), radius, 0xffff0000);

	pt2::RenderSystem::DrawPainter(pt);
}

void WxStageCanvas::DrawForeground() const
{
	if (!m_obj->HasSharedComp<n3::CompModel>()) {
		return;
	}

	pt3::RenderParams params;
	params.mt   = m_camera->GetViewMat();
	params.type = pt3::RenderParams::DRAW_MESH;

    pt3::RenderContext ctx;
    ctx.uniforms.AddVar(
        pt3::MaterialMgr::PositionUniforms::light_pos.name,
        pt0::RenderVariant(sm::vec3(0, 2, -4))
    );

	auto& cmodel_inst = m_obj->GetUniqueComp<n3::CompModelInst>();
	auto& model_inst = cmodel_inst.GetModel();
	if (!model_inst) {
		return;
	}

	auto& model = model_inst->GetModel();
	auto& ext = model->ext;
	if (ext && ext->Type() == ::model::EXT_SKELETAL)
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