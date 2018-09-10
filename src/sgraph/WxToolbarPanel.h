#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <wx/panel.h>

class wxRadioBox;

namespace pt3 { class Material; }
namespace ee0 { class RenderContext; }
namespace ee3 { class WxMaterialPreview; }
namespace sg { class WxNodeProperty; }

namespace eone
{
namespace sgraph
{

class WxToolbarPanel : public wxPanel, public ee0::Observer
{
public:
	WxToolbarPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
		const ee0::RenderContext* rc);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	pt3::Material& GetPreviewMaterial();

	auto GetPreviewPanel() { return m_preview; }

private:
	void InitLayout(const ee0::RenderContext* rc);

	void OnModelTypeChange(wxCommandEvent& event);

	void SetModelType(const std::string& model);

	void OnSelected(const ee0::VariantSet& variants);

private:
	ee0::SubjectMgrPtr m_sub_mgr;

	wxRadioBox* m_model;

	ee3::WxMaterialPreview* m_preview;

	sg::WxNodeProperty* m_prop;

}; // WxToolbarPanel

}
}