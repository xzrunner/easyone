#include "rgraph/WxStageCanvas.h"

#include <ee0/WxStagePage.h>
#include <ee0/MsgHelper.h>
#include <ee0/SubjectMgr.h>
#include <blueprint/CompNode.h>
#include <blueprint/Node.h>
#include <blueprint/NodeHelper.h>

#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>

namespace eone
{
namespace rgraph
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage, const ee0::RenderContext& rc)
	: WxStageCanvas2D(stage, rc)
{
}

bool WxStageCanvas::OnUpdate()
{
	WxStageCanvas2D::OnUpdate();

//	bool bp_dirty = false;
	bool canvas_dirty = false;
	m_stage->Traverse([&](const ee0::GameObj& obj)->bool
	{
		if (obj->HasUniqueComp<bp::CompNode>())
		{
			auto& bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
			if (bp_node->IsLifeDeleteLater())
			{
				ee0::MsgHelper::DeleteNode(*m_stage->GetSubjectMgr(), obj);
//				bp_dirty = true;
				canvas_dirty = true;
				return false;
			}
			if (bp_node->IsSizeChanged())
			{
				bp_node->SetSizeChanged(false);
				auto& st = bp_node->GetStyle();
				sm::rect sz(st.width, st.height);
				obj->GetUniqueComp<n2::CompBoundingBox>().SetSize(*obj, sz);
			}
		}
		return true;
	});

	//if (bp_dirty) {
	//	m_stage->GetSubjectMgr()->NotifyObservers(bp::MSG_BLUE_PRINT_CHANGED);
	//}
	if (canvas_dirty) {
		m_stage->GetSubjectMgr()->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	}
	return canvas_dirty;
}

}
}