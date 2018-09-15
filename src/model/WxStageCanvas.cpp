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
#include <node3/RenderSystem.h>

#include <painting2/PrimitiveDraw.h>
#include <painting3/PrimitiveDraw.h>

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

void WxStageCanvas::DrawBackground() const
{
	static const int LEN = 1;
	pt2::PrimitiveDraw::PointSize(5);
	pt2::PrimitiveDraw::LineWidth(2);
	pt3::PrimitiveDraw::SetColor(0xff0000ff);
	pt3::PrimitiveDraw::Line(sm::vec3(-LEN, 0, 0), sm::vec3(LEN, 0, 0));
	pt3::PrimitiveDraw::Point(sm::vec3(LEN, 0, 0));
	pt3::PrimitiveDraw::SetColor(0xff00ff00);
	pt3::PrimitiveDraw::Line(sm::vec3(0, -LEN, 0), sm::vec3(0, LEN, 0));
	pt3::PrimitiveDraw::Point(sm::vec3(0, LEN, 0));
	pt3::PrimitiveDraw::SetColor(0xffff0000);
	pt3::PrimitiveDraw::Line(sm::vec3(0, 0, -LEN), sm::vec3(0, 0, LEN));
	pt3::PrimitiveDraw::Point(sm::vec3(0, 0, LEN));
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
		RenderSystem::Instance()->DrawModel(*model_inst, params, true);
	} else {
		n3::RenderSystem::Draw(m_obj, params);
	}
}

}
}