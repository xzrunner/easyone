#include "frame/WxPreviewPanel.h"

namespace eone
{

WxPreviewPanel::WxPreviewPanel(wxWindow* parent)
	: ee0::WxStagePage(parent)
{
}

void WxPreviewPanel::Traverse(std::function<bool(const n0::SceneNodePtr&)> func) const
{

}

}