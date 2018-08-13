#pragma once

#include <ee0/Observer.h>
#include <ee0/GuiCallback.h>
#include <ee0/typedef.h>

#include <anim/Layer.h>

#include <wx/panel.h>

namespace ee3 { class WxTransformCtrl; }

namespace eone
{
namespace anim3
{

class WxPropertyPanel : public wxPanel, public ee0::Observer
{
public:
	WxPropertyPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
		const std::vector<::anim::LayerPtr>& layers);
	virtual ~WxPropertyPanel();

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants);

private:
	void InitLayout();

	void OnSetCurrFrame(const ee0::VariantSet& variants);

private:
	ee0::SubjectMgrPtr m_sub_mgr;

	const std::vector<::anim::LayerPtr>& m_layers;

	ee3::WxTransformCtrl* m_trans_ctrl = nullptr;

	n0::SceneNodePtr m_selected = nullptr;

}; // WxPropertyPanel

}
}