#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <wx/panel.h>

namespace rlab { class WxNodeProperty; }

namespace eone
{

class WxStagePage;

namespace rgraph
{

class WxStagePage;

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

    rlab::WxNodeProperty* m_prop;

}; // WxToolbarPanel

}
}