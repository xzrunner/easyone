#include "sgraph/WxToolbarPanel.h"
#include "sgraph/WxStagePage.h"
#include "sgraph/MessageID.h"

#include <ee3/WxMaterialPreview.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <blueprint/CompNode.h>
#include <blueprint/MessageID.h>
#include <shadergraph/WxNodeProperty.h>
#include <shadergraph/RegistNodes.h>

#include <wx/sizer.h>

namespace eone
{
namespace sgraph
{

WxToolbarPanel::WxToolbarPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
	                           const ee0::RenderContext* rc)
	: wxPanel(parent)
	, m_sub_mgr(sub_mgr)
    , m_model_type(ModelType::RAYMARCHING)
{
	InitLayout(rc);

    SetModelType(m_model_type);

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

void WxToolbarPanel::InitLayout(const ee0::RenderContext* rc)
{
	auto sizer = new wxBoxSizer(wxVERTICAL);
	// model
	{
		wxArrayString choices;
		choices.push_back("Sprite");
		choices.push_back("Phong");
		choices.push_back("PBR");
        choices.push_back("Raymarching");
		sizer->Add(m_model_ctrl = new wxRadioBox(this, wxID_ANY, "model_type",
			wxDefaultPosition, wxDefaultSize, choices, 0, wxRA_SPECIFY_COLS));
        m_model_ctrl->SetSelection(static_cast<int>(m_model_type));
		Connect(m_model_ctrl->GetId(), wxEVT_COMMAND_CHOICE_SELECTED,
			wxCommandEventHandler(WxToolbarPanel::OnModelTypeChange));
	}
	sizer->AddSpacer(10);
	// preview
	sizer->Add(m_preview = new ee3::WxMaterialPreview(this, sm::ivec2(300, 300), m_sub_mgr, rc, true));
	sizer->AddSpacer(10);
	// property
	sizer->Add(m_prop = new sg::WxNodeProperty(this, m_sub_mgr));

	SetSizer(sizer);
}

void WxToolbarPanel::OnModelTypeChange(wxCommandEvent& event)
{
    SetModelType(static_cast<ModelType>(event.GetSelection()));
}

void WxToolbarPanel::SetModelType(ModelType type)
{
    std::string str;
    switch (type)
	{
    case ModelType::SPRITE:
        str = rttr::type::get<sg::node::Sprite>().get_name().to_string();
		break;
	case ModelType::PHONG:
        str = rttr::type::get<sg::node::Phong>().get_name().to_string();
		break;
    case ModelType::PBR:
        break;
    case ModelType::RAYMARCHING:
        str = rttr::type::get<sg::node::Raymarching>().get_name().to_string();
        break;
    default:
        return;
	}

    ee0::VariantSet vars;

    ee0::Variant var;
    var.m_type = ee0::VT_PCHAR;
    var.m_val.pc = const_cast<char*>(str.c_str());
    vars.SetVariant("type", var);

    m_sub_mgr->NotifyObservers(MSG_SET_MODEL_TYPE, vars);
}

void WxToolbarPanel::OnSelected(const ee0::VariantSet& variants)
{
	auto var_obj = variants.GetVariant("obj");
	GD_ASSERT(var_obj.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj obj = *static_cast<ee0::GameObj*>(var_obj.m_val.pv);
	GD_ASSERT(GAME_OBJ_VALID(obj), "err scene obj");

	auto& cnode = obj->GetUniqueComp<bp::CompNode>();
	m_prop->LoadFromNode(obj, cnode.GetNode());
}

}
}