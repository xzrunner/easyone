#include "frame/WxStageCanvas3D.h"
#include "frame/WxStagePage.h"

#include <ee0/SubjectMgr.h>

#ifndef GAME_OBJ_ECS
#include <node3/UpdateSystem.h>
#else
#endif // GAME_OBJ_ECS

#include <wx/frame.h>

namespace eone
{

WxStageCanvas3D::WxStageCanvas3D(WxStagePage* stage, ECS_WORLD_PARAM
	                             const ee0::RenderContext& rc, bool has2d)
	: ee3::WxStageCanvas(stage, ECS_WORLD_VAR &rc, nullptr, has2d)
{
}

bool WxStageCanvas3D::OnUpdate()
{
	bool dirty = false;
	m_stage->Traverse([&](const ee0::GameObj& obj)->bool
	{
		if (n3::UpdateSystem::Update(obj)) {
			dirty = true;
		}
		return true;
	});

	if (dirty) {
		m_stage->GetSubjectMgr()->NotifyObservers(ee0::MSG_UPDATE_NODES);
	}
	return dirty;
}

}