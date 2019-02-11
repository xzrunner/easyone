#include "quake/WxPreviewCanvas.h"
#include "quake/WxPreviewPanel.h"

#include <ee0/SubjectMgr.h>

#include <ns/NodeFactory.h>
#include <node0/SceneNode.h>
#include <node3/RenderSystem.h>
#include <node3/CompTransform.h>
#include <painting3/MaterialMgr.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>

namespace eone
{
namespace quake
{

WxPreviewCanvas::WxPreviewCanvas(WxPreviewPanel* stage, ECS_WORLD_PARAM
	                             const ee0::RenderContext& rc)
	: ee3::WxStageCanvas(stage, &rc, nullptr, false)
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
	// todo
	if (!m_node)
	{
		m_node = ns::NodeFactory::Create("C:\\projects\\quake3\\quakespasm-0.93.1\\assets\\Id1\\data\\maps\\start.bsp");
		auto& ctrans = m_node->GetUniqueComp<n3::CompTransform>();
		ctrans.Rotate(sm::Quaternion::CreateFromEulerAngle(0, -SM_PI * 0.5f, 0));
		ctrans.SetScale(sm::vec3(-1, 1, 1));
//		ctrans.Translate(sm::vec3(0, 10, 0));
	}

	pt3::RenderParams params;

    pt3::RenderContext ctx;
    ctx.uniforms.AddVar(
        pt3::MaterialMgr::PositionUniforms::light_pos.name,
        pt0::RenderVariant(sm::vec3(0, 2, -4))
    );
    auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
    assert(wc);
    ctx.uniforms.AddVar(
        pt3::MaterialMgr::PosTransUniforms::view.name,
        pt0::RenderVariant(wc->GetViewMat())
    );
    ctx.uniforms.AddVar(
        pt3::MaterialMgr::PosTransUniforms::projection.name,
        pt0::RenderVariant(wc->GetProjMat())
    );

	n3::RenderSystem::Draw(*m_node, params, ctx);
}

void WxPreviewCanvas::RegisterMsg(ee0::SubjectMgr& sub_mgr)
{
	sub_mgr.RegisterObserver(ee0::MSG_SET_CANVAS_DIRTY, this);
}

}
}