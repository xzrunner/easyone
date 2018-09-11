#include "sgraph/WxStageCanvas.h"

#include <ee0/WxStagePage.h>
#include <ee0/MsgHelper.h>
#include <ee0/SubjectMgr.h>
#include <blueprint/CompNode.h>
#include <blueprint/Node.h>

#include <node0/SceneNode.h>

namespace eone
{
namespace sgraph
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage, const ee0::RenderContext& rc)
	: WxStageCanvas2D(stage, rc)
{
}

bool WxStageCanvas::OnUpdate()
{
	WxStageCanvas2D::OnUpdate();

	bool dirty = false;
	m_stage->Traverse([&](const ee0::GameObj& obj)->bool
	{
		if (obj->HasUniqueComp<bp::CompNode>())
		{
			auto& cnode = obj->GetUniqueComp<bp::CompNode>();
			if (cnode.GetNode()->IsLifeDeleteLater()) {
				ee0::MsgHelper::DeleteNode(*m_stage->GetSubjectMgr(), obj);
				dirty = true;
				return false;
			}
		}
		return true;
	});

	if (dirty) {
		m_stage->GetSubjectMgr()->NotifyObservers(ee0::MSG_UPDATE_NODES);
	}
	return dirty;
}

}
}