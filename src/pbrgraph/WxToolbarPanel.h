#pragma once

#include "frame/config.h"

#ifdef MODULE_PBRGRAPH

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <wx/panel.h>

namespace renderlab { class WxNodeProperty; }

namespace eone
{

class WxStagePage;

namespace pbrgraph
{

class WxStagePage;
class WxPreviewPanel;

class WxToolbarPanel : public wxPanel, public ee0::Observer
{
public:
	WxToolbarPanel(wxWindow* parent, eone::WxStagePage* stage_page);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

private:
	void InitLayout();

    void OnSelected(const ee0::VariantSet& variants);

private:
    eone::WxStagePage* m_stage_page;

    WxPreviewPanel* m_preview;

    renderlab::WxNodeProperty* m_prop;

}; // WxToolbarPanel

}
}

#endif // MODULE_PBRGRAPH