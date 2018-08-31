#include "material/WxToolbarPanel.h"
#include "material/WxStagePage.h"
#include "material/MessageID.h"

#include <ee3/WxMaterialPreview.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <blueprint/CompNode.h>
#include <blueprint/MessageID.h>
#include <ematerial/WxNodeProperty.h>

#include <wx/sizer.h>

namespace eone
{
namespace material
{

WxToolbarPanel::WxToolbarPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr)
	: wxPanel(parent)
	, m_sub_mgr(sub_mgr)
{
	InitLayout();

	SetModelType("mat_phong_model");

	sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
}

void WxToolbarPanel::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_NODE_SELECTION_INSERT:
		OnSelected(variants);
		break;
	}
}

pt3::Material& WxToolbarPanel::GetPreviewMaterial()
{
	assert(m_preview);
	return m_preview->GetMaterial();
}

void WxToolbarPanel::InitLayout()
{
	auto sizer = new wxBoxSizer(wxVERTICAL);
	// model
	{
		wxArrayString choices;
		choices.push_back("Phong");
		choices.push_back("PBR");
		sizer->Add(m_model = new wxRadioBox(this, wxID_ANY, "model_type",
			wxDefaultPosition, wxDefaultSize, choices, 0, wxRA_SPECIFY_COLS));
		Connect(m_model->GetId(), wxEVT_COMMAND_CHOICE_SELECTED,
			wxCommandEventHandler(WxToolbarPanel::OnModelTypeChange));
	}
	sizer->AddSpacer(10);
	// preview
	sizer->Add(m_preview = new ee3::WxMaterialPreview(this, sm::ivec2(300, 300)));
	sizer->AddSpacer(10);
	// property
	sizer->Add(m_prop = new ematerial::WxNodeProperty(this, m_sub_mgr));

	SetSizer(sizer);
}

void WxToolbarPanel::OnModelTypeChange(wxCommandEvent& event)
{
	switch (event.GetSelection())
	{
	case 0:
		SetModelType("mat_phong_model");
		break;
	}
}

void WxToolbarPanel::SetModelType(const std::string& model)
{
	ee0::VariantSet vars;

	ee0::Variant var;
	var.m_type = ee0::VT_PCHAR;
	var.m_val.pc = const_cast<char*>(model.c_str());
	vars.SetVariant("type", var);

	m_sub_mgr->NotifyObservers(MSG_SET_MODEL_TYPE, vars);
}

void WxToolbarPanel::OnSelected(const ee0::VariantSet& variants)
{
	auto var_obj = variants.GetVariant("obj");
	GD_ASSERT(var_obj.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj obj = *static_cast<ee0::GameObj*>(var_obj.m_val.pv);
	GD_ASSERT(GAME_OBJ_VALID(obj), "err scene obj");

	auto& cnode = obj->GetSharedComp<bp::CompNode>();
	m_prop->LoadFromNode(cnode.GetNode());
}

}
}