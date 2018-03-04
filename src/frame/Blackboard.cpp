#include "frame/Blackboard.h"

#include <ee0/WxStageCanvas.h>

#include <wx/frame.h>

namespace eone
{

CU_SINGLETON_DEFINITION(Blackboard);

Blackboard::Blackboard()
	: m_frame(nullptr)
	, m_library(nullptr)
	, m_stage(nullptr)
	, m_preview(nullptr)
	, m_dummy_canvas(nullptr)
{
}

void Blackboard::InitRenderContext()
{
	m_dummy_canvas = ee0::WxStageCanvas::CreateWxGLCanvas(m_frame);
	ee0::WxStageCanvas::CreateRenderContext(m_rc, m_dummy_canvas);
	ee0::WxStageCanvas::CreateWindowContext(m_wc, true, false);
}

}