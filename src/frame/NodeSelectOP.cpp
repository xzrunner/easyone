#include "frame/NodeSelectOP.h"
#include "frame/Blackboard.h"

#include "mask/WxEditDialog.h"
#include "scale9/WxEditDialog.h"

#include <ee0/WxStagePage.h>
#include <ee0/WxStageCanvas.h>
#include <ee0/EditOpState.h>

#include <node0/SceneNode.h>
#include <node2/CompMask.h>
#include <node2/CompScale9.h>

namespace
{

void OpenEditDialog(ee0::WxStagePage& stage, const n0::SceneNodePtr& node,
	                const ee0::RenderContext& rc, const ee0::WindowContext& wc)
{
	if (node->HasComponent<n2::CompMask>())
	{
		auto& canvas = stage.GetImpl().GetCanvas();
		eone::mask::WxEditDialog dlg(eone::Blackboard::Instance()->GetStage(), rc, wc, node);
		dlg.ShowModal();
	}
	else if (node->HasComponent<n2::CompScale9>())
	{
		auto& canvas = stage.GetImpl().GetCanvas();
		eone::scale9::WxEditDialog dlg(eone::Blackboard::Instance()->GetStage(), rc, wc, node);
		dlg.ShowModal();
	}
}

}

namespace eone
{

NodeSelectOP::NodeSelectOP(ee0::WxStagePage& stage,
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

	selection.Traverse(
		[&](const n0::SceneNodePtr& node)->bool
		{
			OpenEditDialog(m_stage, node, m_rc, m_wc);
			return false;
		}
	);

	return false;
}

}