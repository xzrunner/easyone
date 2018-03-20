#include "script/WxStageCanvas.h"

#include "frame/WxStagePage.h"

#include <painting2/PrimitiveDraw.h>
#include <dust/Context.h>
#include <dust/Blackboard.h>
#include <ee0/SubjectMgr.h>

namespace eone
{
namespace script
{
	
WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage,
	                         const ee0::RenderContext& rc,
	                         const std::string& filepath)
	: ee2::WxStageCanvas(stage, &rc)
	, m_script(stage->GetDustCtx()->GetState(), filepath.c_str())
{
	stage->GetSubjectMgr()->RegisterObserver(ee0::MSG_EDITOR_RELOAD, this);
}

void WxStageCanvas::ScriptLoad()
{
	SetCurrentCanvas();
	BindDustCtx();

	m_script.OnLoad();
}

void WxStageCanvas::OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants)
{
	BindDustCtx();

	ee2::WxStageCanvas::OnNotify(msg, variants);

	switch (msg)
	{
	case ee0::MSG_EDITOR_RELOAD:
		m_script.LoadScript();
		m_script.OnLoad();
		SetDirty();
		break;
	}
}

void WxStageCanvas::DrawBackground() const
{
	BindDustCtx();
	m_script.OnDraw();
}

void WxStageCanvas::OnTimer()
{
	BindDustCtx();
	m_script.OnUpdate();
	SetDirty();
}

void WxStageCanvas::BindDustCtx() const
{
	dust::Blackboard::Instance()->SetContext(m_stage->GetDustCtx());
}

}
}