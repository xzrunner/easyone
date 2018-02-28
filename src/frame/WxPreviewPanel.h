#pragma once

#include <ee0/WxStagePage.h>

namespace eone
{

class WxPreviewPanel : public ee0::WxStagePage
{
public:
	WxPreviewPanel(wxWindow* parent);

	virtual void Traverse(std::function<bool(const n0::SceneNodePtr&)> func) const override;

}; // WxPreviewPanel

}