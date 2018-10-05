#include "script/WxStageCanvas.h"

#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"

#include <ee0/SubjectMgr.h>
#include <ee0/CameraHelper.h>

#include <painting2/PrimitiveDraw.h>
#include <moon/Context.h>
#include <moon/Blackboard.h>

namespace
{

const uint32_t MESSAGES[] =
{
	ee0::MSG_EDITOR_RELOAD,
};

}

namespace eone
{
namespace script
{
	
WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage,
	                         ECS_WORLD_PARAM
	                         const ee0::RenderContext& rc,
	                         const std::string& filepath)
	: ee2::WxStageCanvas(stage, ECS_WORLD_VAR &rc)
	, m_script(stage->GetMoonCtx()->GetState(), filepath.c_str())
{
	for (auto& msg : MESSAGES) {
		stage->GetSubjectMgr()->RegisterObserver(msg, this);
	}
}

WxStageCanvas::~WxStageCanvas()
{
	for (auto& msg : MESSAGES) {
		m_stage->GetSubjectMgr()->UnregisterObserver(msg, this);
	}
}

void WxStageCanvas::ScriptLoad()
{
	SetCurrentCanvas();
	BindMoonCtx();

	m_script.OnLoad();
}

void WxStageCanvas::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	BindMoonCtx();

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
	BindMoonCtx();
	m_script.OnDraw();
}

void WxStageCanvas::OnTimer()
{
	BindMoonCtx();
	m_script.OnUpdate();
	SetDirty();
}

void WxStageCanvas::OnMouseImpl(wxMouseEvent& event)
{
	ee2::WxStageCanvas::OnMouseImpl(event);

	int x = event.GetX();
	int y = event.GetY();
	sm::vec2 pos = ee0::CameraHelper::TransPosScreenToProject(*GetCamera(), x, y);

	wxString msg;
	msg.Printf("Mouse: %.1f, %.1f", pos.x, pos.y);
	Blackboard::Instance()->GetFrame()->SetStatusText(msg);

	BindMoonCtx();
	if (event.LeftDown()) {
		m_script.OnMousePressed(x, y, 1);
	} else if (event.LeftUp()) {
		m_script.OnMouseReleased(x, y, 1);
	} else if (event.RightDown()) {
		m_script.OnMousePressed(x, y, 2);
	} else if (event.RightUp()) {
		m_script.OnMouseReleased(x, y, 2);
	}
}

void WxStageCanvas::OnKeyDownImpl(wxKeyEvent& event)
{
	ee2::WxStageCanvas::OnKeyDownImpl(event);

	int key_code = event.GetKeyCode();
	if (key_code >= 'A' && key_code <= 'Z') 
	{
		std::string str;
		str.push_back(static_cast<char>('a' + key_code - 'A'));
		m_script.OnKeyPressed(str.c_str());
		return;
	}
	else if (key_code >= '0' && key_code <= '9')
	{
		std::string str;
		str.push_back(static_cast<char>('0' + key_code - '0'));
		m_script.OnKeyPressed(str.c_str());
		return;
	}

	switch (key_code)
	{
	case WXK_LEFT:
		m_script.OnKeyPressed("left");
		break;
	case WXK_UP:
		m_script.OnKeyPressed("up");
		break;
	case WXK_RIGHT:
		m_script.OnKeyPressed("right");
		break;
	case WXK_DOWN:
		m_script.OnKeyPressed("down");
		break;
	}
}

void WxStageCanvas::BindMoonCtx() const
{
	moon::Blackboard::Instance()->SetContext(m_stage->GetMoonCtx());
}

}
}