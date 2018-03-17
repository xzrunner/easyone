#include "script/WxStageCanvas.h"

#include "frame/WxStagePage.h"

#include <painting2/PrimitiveDraw.h>
#include <dust/LuaVM.h>

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
	m_script.Load();
}

void WxStageCanvas::OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants)
{
	ee2::WxStageCanvas::OnNotify(msg, variants);

	switch (msg)
	{
	case ee0::MSG_EDITOR_RELOAD:
		break;
	}
}

void WxStageCanvas::DrawBackground() const
{
	m_script.Draw();
}

void WxStageCanvas::OnTimer()
{
	m_script.Update();
}

}
}