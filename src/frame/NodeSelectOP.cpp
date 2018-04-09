#include "frame/NodeSelectOP.h"
#include "frame/Blackboard.h"
#include "frame/WxStagePage.h"
#include "frame/Application.h"
#include "frame/WxStagePanel.h"

#include "mask/WxEditDialog.h"
#include "scale9/WxEditDialog.h"

#include <ee0/WxStageCanvas.h>
#include <ee0/EditOpState.h>

#include <node0/SceneNode.h>
#include <node2/CompMask.h>
#include <node2/CompScale9.h>

namespace
{

void OpenEditDialog(ee0::WxStagePage& stage, const ee0::GameObj& obj,
	                const ee0::RenderContext& rc, const ee0::WindowContext& wc)
{
	auto stage_panel = eone::Blackboard::Instance()->GetApp()->GetStagePanel();
	if (obj->HasSharedComp<n2::CompMask>())
	{
		auto& canvas = stage.GetImpl().GetCanvas();
		eone::mask::WxEditDialog dlg(stage_panel, rc, wc, obj);
		dlg.ShowModal();
	}
	else if (obj->HasSharedComp<n2::CompScale9>())
	{
		auto& canvas = stage.GetImpl().GetCanvas();
		eone::scale9::WxEditDialog dlg(stage_panel, rc, wc, obj);
		dlg.ShowModal();
	}
}

}

namespace eone
{

NodeSelectOP::NodeSelectOP(WxStagePage& stage,
	                       const ee0::RenderContext& rc,
	                       const ee0::WindowContext& wc)
	: ee2::NodeSelectOP(stage)
	, m_rc(rc)
	, m_wc(wc)
{
}

bool NodeSelectOP::OnMouseLeftDClick(int x, int y)
{
	if (ee2::NodeSelectOP::OnMouseLeftDClick(x, y)) {
		return true;
	}

	auto& selection = m_stage.GetNodeSelection();
	if (selection.Size() != 1) {
		return false;
	}

	selection.Traverse([&](const n0::NodeWithPos& nwp)->bool {
		OpenEditDialog(m_stage, nwp.GetNode(), m_rc, m_wc);
		return false;
	});

	return false;
}

}