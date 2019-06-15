#include "anim3/WxPropertyPanel.h"

#ifdef MODULE_ANIM3

#include "anim3/CompBone.h"

#include <ee0/SubjectMgr.h>
#include <ee3/WxTransformCtrl.h>
#include <eanim/MessageID.h>

#include <guard/check.h>
#include <node0/SceneNode.h>

#include <wx/sizer.h>

namespace
{

const uint32_t MESSAGES[] =
{
	eanim::MSG_SET_CURR_FRAME,
};

const sm::vec3       DEFAULT_POSITION(0, 0, 0);
const sm::Quaternion DEFAULT_ROTATION;
const sm::vec3       DEFAULT_SCALING(1, 1, 1);

}

namespace eone
{
namespace anim3
{

WxPropertyPanel::WxPropertyPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
	                             const std::vector<::anim::LayerPtr>& layers)
	: wxPanel(parent)
	, m_sub_mgr(sub_mgr)
	, m_layers(layers)
{
	for (auto& msg : MESSAGES) {
		m_sub_mgr->RegisterObserver(msg, this);
	}

	InitLayout();
}

WxPropertyPanel::~WxPropertyPanel()
{
	for (auto& msg : MESSAGES) {
		m_sub_mgr->UnregisterObserver(msg, this);
	}
}

void WxPropertyPanel::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case eanim::MSG_SET_CURR_FRAME:
		OnSetCurrFrame(variants);
		break;
	}
}

void WxPropertyPanel::InitLayout()
{
	ee3::WxTransformCtrl::Callback cb;
	cb.get_position = [&]()->const sm::vec3& {
		return m_selected ?
			m_selected->GetSharedComp<CompBone>().GetPosition() :
			DEFAULT_POSITION;
	};
	cb.get_rotation = [&]()->const sm::Quaternion& {
		return m_selected ?
			m_selected->GetSharedComp<CompBone>().GetRotation() :
			DEFAULT_ROTATION;
	};
	cb.get_scaling = [&]()->const sm::vec3& {
		return m_selected ?
			m_selected->GetSharedComp<CompBone>().GetScaling() :
			DEFAULT_SCALING;
	};
	cb.set_position = [&](const sm::vec3& pos) {
		if (m_selected) {
			m_selected->GetSharedComp<CompBone>().SetPosition(pos);
		}
	};
	cb.set_rotation = [&](const sm::Quaternion& rot) {
		if (m_selected) {
			m_selected->GetSharedComp<CompBone>().SetRotation(rot);
		}
	};
	cb.set_scaling = [&](const sm::vec3& scale) {
		if (m_selected) {
			m_selected->GetSharedComp<CompBone>().SetScaling(scale);
		}
	};

	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_trans_ctrl = new ee3::WxTransformCtrl(this, m_sub_mgr, cb));
	SetSizer(sizer);
}

void WxPropertyPanel::OnSetCurrFrame(const ee0::VariantSet& variants)
{
	auto old = m_selected;
	m_selected = nullptr;

	auto var_layer = variants.GetVariant("layer");
	GD_ASSERT(var_layer.m_type == ee0::VT_INT, "err layer");
	int ilayer = var_layer.m_val.l;
	if (ilayer < 0 || ilayer >= static_cast<int>(m_layers.size())) {
		return;
	}

	auto& layer = m_layers[ilayer];

	auto var_frame = variants.GetVariant("frame");
	GD_ASSERT(var_frame.m_type == ee0::VT_INT, "err frame");
	int iframe = var_frame.m_val.l;
	if (iframe < 0 || iframe > layer->GetMaxFrameIdx()) {
		return;
	}

	auto key_frame = layer->GetCurrKeyFrame(iframe);
	assert(key_frame->GetAllNodes().size() == 1);
	m_selected = key_frame->GetAllNodes()[0];

	if (m_selected != old) {
		m_trans_ctrl->RefreshView();
	}
}

}
}

#endif // MODULE_ANIM3