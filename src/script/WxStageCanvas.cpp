#include "script/WxStageCanvas.h"

#include "frame/WxStagePage.h"

#include <painting2/PrimitiveDraw.h>
#include <dust/LuaVM.h>
#include <ee0/SubjectMgr.h>

namespace eone
{
namespace script
{
	
WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage,
	                         const ee0::RenderContext& rc,
	                         const std::string& filepath)
	: ee2::WxStageCanvas(stage, &rc)
	, m_script(stage->GetLuaVM()->GetState(), filepath.c_str())
{
	stage->GetSubjectMgr()->RegisterObserver(ee0::MSG_EDITOR_RELOAD, this);

	m_script.OnLoad();
}

void WxStageCanvas::OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants)
{
	ee2::WxStageCanvas::OnNotify(msg, variants);

	switch (msg)
	{
	case ee0::MSG_EDITOR_RELOAD:
		m_script.LoadScript();
		break;
	}
}

void WxStageCanvas::DrawBackground() const
{
	m_script.OnDraw();
}

void WxStageCanvas::OnTimer()
{
	m_script.OnUpdate();
}

}
}