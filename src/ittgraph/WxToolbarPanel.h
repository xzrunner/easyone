#pragma once

#include "frame/config.h"

#ifdef MODULE_ITTGRAPH

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <wx/panel.h>

namespace itt { class WxNodeProperty; }

namespace eone
{

class WxStagePage;

namespace ittgraph
{

class WxToolbarPanel : public wxPanel, public ee0::Observer
{
public:
	WxToolbarPanel(wxWindow* parent, eone::WxStagePage* stage_page);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

private:
	void InitLayout();

    void OnSelectionInsert(const ee0::VariantSet& variants);
    void OnSelectionClear(const ee0::VariantSet& variants);

private:
    eone::WxStagePage* m_stage_page;

    itt::WxNodeProperty* m_prop;

}; // WxToolbarPanel

}
}

#endif // MODULE_ITTGRAPH