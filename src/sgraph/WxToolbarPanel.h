#pragma once

#include "frame/config.h"

#ifdef MODULE_SHADERGRAPH

#include "sgraph/ModelType.h"

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <wx/panel.h>

class wxRadioBox;

namespace ee0 { class RenderContext; }
namespace ee3 { class WxMaterialPreview; }
namespace sg { class WxNodeProperty; }

namespace eone
{
namespace sgraph
{

class WxStagePage;

class WxToolbarPanel : public wxPanel, public ee0::Observer
{
public:
	WxToolbarPanel(wxWindow* parent, WxStagePage* stage_page);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	//pt3::Material& GetPreviewMaterial();

	auto GetPreviewPanel() { return m_preview; }

private:
	void InitLayout();

	void OnModelTypeChange(wxCommandEvent& event);

	void OnSelected(const ee0::VariantSet& variants);

private:
    WxStagePage* m_stage_page;

	wxRadioBox* m_model_ctrl;

	ee3::WxMaterialPreview* m_preview;

	sg::WxNodeProperty* m_prop;

}; // WxToolbarPanel

}
}

#endif // MODULE_SHADERGRAPH