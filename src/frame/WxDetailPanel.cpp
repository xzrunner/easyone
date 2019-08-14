#include "frame/WxDetailPanel.h"
#include "frame/WxStagePage.h"
#include "frame/config.h"

#include <ee0/WxListSelectDlg.h>
#include <ee0/SubjectMgr.h>
#include <ee0/VariantSet.h>
#include <ee0/WxCompPanel.h>
#include <ee0/WxCompFlagPanel.h>
#include <ee0/WxCompIdPanel.h>
#include <ee0/CompCustomProp.h>
#include <ee0/WxCompCustomPropPanel.h>
#include <ee2/WxCompTransformPanel.h>
#include <ee2/WxCompColComPanel.h>
#include <ee2/WxCompColMapPanel.h>
#include <ee2/WxCompImagePanel.h>
#include <ee2/WxCompTextPanel.h>
#include <ee2/WxCompMaskPanel.h>
#include <ee2/WxCompMeshPanel.h>
#include <ee2/WxCompScale9Panel.h>
#include <ee2/WxCompScissorPanel.h>
#include <ee2/WxCompScriptPanel.h>
#include <ee2/WxCompShapePanel.h>
#include <ee3/WxCompTransformPanel.h>
#include <ee3/WxCompModelPanel.h>
#include <ee3/WxCompLightPanel.h>
#include <ee3/WxCompMeshFilterPanel.h>

#include <guard/check.h>
#include <moon/Context.h>
#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node0/CompIdentity.h>
#include <node0/CompFlags.h>
#include <node2/CompImage.h>
#include <node2/CompText.h>
#include <node2/CompMask.h>
#include <node2/CompMesh.h>
#include <node2/CompScale9.h>
#include <node2/CompScissor.h>
#include <node2/CompScript.h>
#include <node2/CompTransform.h>
#include <node2/CompShape.h>
#include <node3/CompTransform.h>
#include <node3/CompModel.h>
#include <node3/CompLight.h>
#include <node3/CompMeshFilter.h>
#else
#include <entity0/World.h>
#include <entity2/CompTransform.h>
#include <entity2/CompImage.h>
#include <entity2/CompText.h>
#include <entity2/CompMask.h>
#include <entity2/CompMesh.h>
#include <entity2/CompScale9.h>
#include <entity2/CompScissor.h>
#include <entity2/CompScript.h>
#include <entity3/CompTransform.h>
#endif // GAME_OBJ_ECS
#include <geoshape/Circle.h>
#include <geoshape/Rect.h>

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/treectrl.h>
#include <wx/dialog.h>

namespace eone
{

WxDetailPanel::WxDetailPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
	                         ECS_WORLD_PARAM const ee0::GameObj& root_obj,
	                         const moon::ContextPtr& moon_ctx)
	: wxPanel(parent, wxID_ANY)
	, m_sub_mgr(sub_mgr)
	ECS_WORLD_SELF_ASSIGN
	, m_root_obj(root_obj)
	, m_moon_ctx(moon_ctx)
{
	SetBackgroundColour(PANEL_COLOR);

	InitLayout();
	RegisterMsg(*m_sub_mgr);
	InitComponents();
}

void WxDetailPanel::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_NODE_SELECTION_INSERT:
		ClearComponents();
		InitComponents(variants);
		break;
	case ee0::MSG_SCENE_NODE_DELETE:
	case ee0::MSG_SCENE_NODE_CLEAR:
//	case ee0::MSG_NODE_SELECTION_CLEAR:
		ClearComponents();
		InitComponents();
		break;
	case ee0::MSG_UPDATE_COMPONENTS:
		UpdateComponents();
		break;
	case ee0::MSG_STAGE_PAGE_CHANGED:
		ClearComponents();
		StagePageChanged(variants);
		InitComponents();
		break;
	}
}

void WxDetailPanel::InitLayout()
{
	wxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);

	InitHeader(top_sizer);

	m_comp_sizer = new wxBoxSizer(wxVERTICAL);
	top_sizer->Add(m_comp_sizer);

	SetSizer(top_sizer);
}

void WxDetailPanel::RegisterMsg(ee0::SubjectMgr& sub_mgr)
{
	sub_mgr.RegisterObserver(ee0::MSG_SCENE_NODE_DELETE, this);
	sub_mgr.RegisterObserver(ee0::MSG_SCENE_NODE_CLEAR, this);

	sub_mgr.RegisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
	sub_mgr.RegisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);

	sub_mgr.RegisterObserver(ee0::MSG_UPDATE_COMPONENTS, this);
	sub_mgr.RegisterObserver(ee0::MSG_STAGE_PAGE_CHANGED, this);
}

void WxDetailPanel::InitHeader(wxSizer* top_sizer)
{
	wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

	m_add_btn = new wxButton(this, wxID_ANY, "+", wxDefaultPosition, wxSize(20, 20));
	Connect(m_add_btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(WxDetailPanel::OnAddPress));
	sizer->Add(m_add_btn, 0, wxLEFT | wxRIGHT, 5);

	top_sizer->Add(sizer);
}

void WxDetailPanel::InitComponents(const ee0::VariantSet& variants)
{
	ee0::GameObj obj, root;
	size_t obj_id = 0;

	auto var_obj = variants.GetVariant("obj");
	GD_ASSERT(var_obj.m_type == ee0::VT_PVOID, "no var in vars: obj");
	obj = *static_cast<const ee0::GameObj*>(var_obj.m_val.pv);
	GD_ASSERT(GAME_OBJ_VALID(obj), "err scene obj");

	auto var_root = variants.GetVariant("root");
	if (var_root.m_type != ee0::VT_EMPTY) {
		GD_ASSERT(var_root.m_type == ee0::VT_PVOID, "no var in vars: obj");
		root = *static_cast<const ee0::GameObj*>(var_root.m_val.pv);
	}

	auto var_id = variants.GetVariant("id");
	if (var_id.m_type != ee0::VT_EMPTY) {
		GD_ASSERT(var_id.m_type == ee0::VT_ULONG, "no var in vars: obj");
		obj_id = var_id.m_val.ul;
	}

#ifndef GAME_OBJ_ECS
	m_owp.Init(obj, root, obj_id);
#endif // GAME_OBJ_ECS

	InitComponents(obj);
}

void WxDetailPanel::InitComponents(const ee0::GameObj& obj)
{
#ifndef GAME_OBJ_ECS
	if (m_owp.GetNode()->HasUniqueComp<n0::CompIdentity>())
#else
#endif // GAME_OBJ_ECS
	{
		auto panel = new ee0::WxCompIdPanel(
#ifndef GAME_OBJ_ECS
			this, m_sub_mgr, m_owp.GetNode()
#else
#endif // GAME_OBJ_ECS
		);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

#ifndef GAME_OBJ_ECS
	if (m_owp.GetNode()->HasUniqueComp<n0::CompFlags>())
#else
	if (m_world.HasComponent<ee0::CompEntityEditor>(m_owp))
#endif // GAME_OBJ_ECS
	{
		auto panel = new ee0::WxCompFlagPanel(
#ifndef GAME_OBJ_ECS
			this, m_sub_mgr, m_owp.GetNode()
#else
			this, m_sub_mgr, m_world, m_owp
#endif // GAME_OBJ_ECS
		);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

#ifndef GAME_OBJ_ECS
	if (m_owp.GetNode()->HasUniqueComp<n3::CompTransform>())
#else
	if (m_world.HasComponent<e3::CompPosition>(m_owp) ||
		m_world.HasComponent<e3::CompAngle>(m_owp) ||
		m_world.HasComponent<e3::CompScale>(m_owp))
#endif // GAME_OBJ_ECS
	{
		auto panel = new ee3::WxCompTransformPanel(
#ifndef GAME_OBJ_ECS
			this, m_sub_mgr, m_owp.GetNode()
#else
			this, m_sub_mgr, m_world, m_owp
#endif // GAME_OBJ_ECS
		);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
    if (m_owp.GetNode()->HasSharedComp<n3::CompModel>())
    {
		auto panel = new ee3::WxCompModelPanel(
			this, m_sub_mgr, m_owp.GetNode()
		);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
    }
    if (m_owp.GetNode()->HasUniqueComp<n3::CompLight>())
    {
        auto panel = new ee3::WxCompLightPanel(
            this, m_sub_mgr, m_owp.GetNode()
        );
        m_comp_sizer->Insert(m_components.size(), panel);
        m_components.push_back(panel);
    }
    if (m_owp.GetNode()->HasUniqueComp<n3::CompMeshFilter>())
    {
        auto panel = new ee3::WxCompMeshFilterPanel(
            this, m_sub_mgr, m_owp.GetNode()
        );
        m_comp_sizer->Insert(m_components.size(), panel);
        m_components.push_back(panel);
    }

#ifndef GAME_OBJ_ECS
	if (m_owp.GetNode()->HasUniqueComp<n2::CompTransform>())
#else
	if (m_world.HasComponent<e2::CompPosition>(m_owp) ||
		m_world.HasComponent<e2::CompAngle>(m_owp) ||
		m_world.HasComponent<e2::CompScale>(m_owp) ||
		m_world.HasComponent<e2::CompShear>(m_owp) ||
		m_world.HasComponent<e2::CompOffset>(m_owp))
#endif // GAME_OBJ_ECS
	{
		auto panel = new ee2::WxCompTransformPanel(
#ifndef GAME_OBJ_ECS
			this, m_sub_mgr, m_owp
#else
			this, m_sub_mgr, m_world, m_owp
#endif // GAME_OBJ_ECS
		);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
#ifndef GAME_OBJ_ECS
	if (m_owp.GetNode()->HasUniqueComp<n2::CompColorCommon>())
#else
	if (m_world.HasComponent<e2::CompColorCommon>(m_owp))
#endif // GAME_OBJ_ECS
	{
#ifndef GAME_OBJ_ECS
		auto& comp = m_owp.GetNode()->GetUniqueComp<n2::CompColorCommon>();
#else
		auto& comp = m_world.GetComponent<e2::CompColorCommon>(m_owp);
#endif // GAME_OBJ_ECS
		auto panel = new ee2::WxCompColComPanel(this, comp, m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
#ifndef GAME_OBJ_ECS
	if (m_owp.GetNode()->HasUniqueComp<n2::CompColorMap>())
#else
	if (m_world.HasComponent<e2::CompColorMap>(m_owp))
#endif // GAME_OBJ_ECS
	{
#ifndef GAME_OBJ_ECS
		auto& comp = m_owp.GetNode()->GetUniqueComp<n2::CompColorMap>();
#else
		auto& comp = m_world.GetComponent<e2::CompColorMap>(m_owp);
#endif // GAME_OBJ_ECS
		auto panel = new ee2::WxCompColMapPanel(this, comp, m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

#ifndef GAME_OBJ_ECS
	if (m_owp.GetNode()->HasSharedComp<n2::CompImage>())
#else
	if (m_world.HasComponent<e2::CompImage>(m_owp))
#endif // GAME_OBJ_ECS
	{
		auto panel = new ee2::WxCompImagePanel(
#ifndef GAME_OBJ_ECS
			this, m_owp.GetNode()
#else
			this, m_world, m_owp
#endif // GAME_OBJ_ECS
		);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
#ifndef GAME_OBJ_ECS
	if (m_owp.GetNode()->HasSharedComp<n2::CompText>())
#else
	if (m_world.HasComponent<e2::CompText>(m_owp))
#endif // GAME_OBJ_ECS
	{
		auto panel = new ee2::WxCompTextPanel(
#ifndef GAME_OBJ_ECS
			this, m_owp.GetNode(), m_sub_mgr
#else
			this, m_world, m_owp, m_sub_mgr
#endif // GAME_OBJ_ECS
		);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
#ifndef GAME_OBJ_ECS
	if (m_owp.GetNode()->HasSharedComp<n2::CompMask>())
#else
	if (m_world.HasComponent<e2::CompMask>(m_owp))
#endif // GAME_OBJ_ECS
	{
		auto panel = new ee2::WxCompMaskPanel(
#ifndef GAME_OBJ_ECS
			this, m_owp.GetNode()
#else
			this, m_world, m_owp
#endif // GAME_OBJ_ECS
		);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
#ifndef GAME_OBJ_ECS
	if (m_owp.GetNode()->HasSharedComp<n2::CompMesh>())
#else
	if (m_world.HasComponent<e2::CompMesh>(m_owp))
#endif // GAME_OBJ_ECS
	{
		auto panel = new ee2::WxCompMeshPanel(
#ifndef GAME_OBJ_ECS
			this, m_owp.GetNode()
#else
			this, m_world, m_owp
#endif // GAME_OBJ_ECS
		);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
#ifndef GAME_OBJ_ECS
	if (m_owp.GetNode()->HasSharedComp<n2::CompScale9>())
#else
	if (m_world.HasComponent<e2::CompScale9>(m_owp))
#endif // GAME_OBJ_ECS
	{
		auto panel = new ee2::WxCompScale9Panel(
#ifndef GAME_OBJ_ECS
			this, m_owp.GetNode()
#else
			this, m_world, m_owp
#endif // GAME_OBJ_ECS
		);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

#ifndef GAME_OBJ_ECS
	if (m_owp.GetNode()->HasUniqueComp<n2::CompScissor>())
#else
	if (m_world.HasComponent<e2::CompScissor>(m_owp))
#endif // GAME_OBJ_ECS
	{
#ifndef GAME_OBJ_ECS
		auto& comp = m_owp.GetNode()->GetUniqueComp<n2::CompScissor>();
#else
		auto& comp = m_world.GetComponent<e2::CompScissor>(m_owp);
#endif // GAME_OBJ_ECS
		auto panel = new ee2::WxCompScissorPanel(this, comp, m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

#ifndef GAME_OBJ_ECS
	if (m_owp.GetNode()->HasUniqueComp<n2::CompScript>())
#else
	if (m_world.HasComponent<e2::CompScript>(m_owp))
#endif // GAME_OBJ_ECS
	{
		auto panel = new ee2::WxCompScriptPanel(
#ifndef GAME_OBJ_ECS
			this, m_sub_mgr, m_owp.GetNode()
#else
			this, m_sub_mgr, m_world, m_owp
#endif // GAME_OBJ_ECS
		);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

	if (m_owp.GetNode()->HasSharedComp<n2::CompShape>())
	{
		auto& comp = m_owp.GetNode()->GetSharedComp<n2::CompShape>();
		auto panel = new ee2::WxCompShapePanel(this, comp);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

#ifndef GAME_OBJ_ECS
	if (m_owp.GetNode()->HasUniqueComp<ee0::CompCustomProp>())
#else
	if (m_world.HasComponent<ee0::CompCustomProp>(m_owp))
#endif // GAME_OBJ_ECS
	{
		auto node = m_owp.GetNode();
		if (!node->HasUniqueComp<ee0::CompCustomProp>()) {
			node->AddUniqueComp<ee0::CompCustomProp>();
		}
		auto& cprop = node->GetUniqueComp<ee0::CompCustomProp>();
		auto panel = new ee0::WxCompCustomPropPanel(this, cprop);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

	Layout();
}

void WxDetailPanel::InitComponents()
{
#ifndef GAME_OBJ_ECS
	m_owp.Init(m_root_obj, m_root_obj, 0);
#endif // GAME_OBJ_ECS
	InitComponents(m_root_obj);
}

void WxDetailPanel::ClearComponents()
{
#ifndef GAME_OBJ_ECS
	m_owp.Reset();
#endif // GAME_OBJ_ECS
	if (m_children.empty()) {
		return;
	}
	for (int i = 0, n = m_components.size(); i < n; ++i) {
		m_comp_sizer->Detach(m_components[i]);
		delete m_components[i];
	}
	m_components.clear();

	Layout();
}

void WxDetailPanel::UpdateComponents()
{
#ifndef GAME_OBJ_ECS
	if (!m_owp.GetNode()) {
#else
	if (!m_owp.IsNull()) {
#endif // GAME_OBJ_ECS
		return;
	}
	for (auto& comp : m_components) {
		comp->RefreshNodeComp();
	}
}

void WxDetailPanel::StagePageChanged(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("new_page");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: new_page");
	GD_ASSERT(var.m_val.pv, "err new_page");

	auto new_page = static_cast<const WxStagePage*>(var.m_val.pv);

	m_sub_mgr = new_page->GetSubjectMgr();
	RegisterMsg(*m_sub_mgr);

	m_root_obj = new_page->GetEditedObj();

	m_moon_ctx = new_page->GetMoonCtx();
}

void WxDetailPanel::OnAddPress(wxCommandEvent& event)
{
	enum CompType
	{
		COMP_UNKNOWN = 0,

		COMP_COLOR_COMMON = 1,
		COMP_COLOR_MAP,
		COMP_SCISSOR,
		COMP_SCRIPT,
		COMP_CIRCLE,
		COMP_RECT,
		COMP_CUSTOM_PROPERTIES,
	};

	struct CompItemData : public wxTreeItemData
	{
		CompItemData(int type)
			: type(type) {}

		int type;

	}; // CompItemData

	const std::vector<std::pair<std::string, wxTreeItemData*>> COMP_LIST =
	{
		{ "ColorCommon",      new CompItemData(COMP_COLOR_COMMON) },
		{ "ColorMap",         new CompItemData(COMP_COLOR_MAP) },
		{ "Scissor",          new CompItemData(COMP_SCISSOR) },
		{ "Script",           new CompItemData(COMP_SCRIPT) },
		{ "Shape Circle",     new CompItemData(COMP_CIRCLE) },
		{ "Shape Rect",       new CompItemData(COMP_RECT) },
		{ "CustomProperties", new CompItemData(COMP_CUSTOM_PROPERTIES) },
	};

	ee0::WxListSelectDlg dlg(this, "Create component",
		COMP_LIST, m_add_btn->GetScreenPosition());
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	auto type = static_cast<CompItemData*>(dlg.GetSelected())->type;
	switch (type)
	{
	case COMP_COLOR_COMMON:
		{
#ifndef GAME_OBJ_ECS
			auto& comp = m_owp.GetNode()->AddUniqueComp<n2::CompColorCommon>();
#else
		    auto& comp = m_world.GetComponent<e2::CompColorCommon>(m_owp);
#endif // GAME_OBJ_ECS
			auto panel = new ee2::WxCompColComPanel(this, comp, m_sub_mgr);
			m_comp_sizer->Insert(m_components.size(), panel);
			m_components.push_back(panel);
		}
		break;
	case COMP_COLOR_MAP:
		{
#ifndef GAME_OBJ_ECS
			auto& comp = m_owp.GetNode()->AddUniqueComp<n2::CompColorMap>();
#else
			auto& comp = m_world.GetComponent<e2::CompColorMap>(m_owp);
#endif // GAME_OBJ_ECS
			auto panel = new ee2::WxCompColMapPanel(this, comp, m_sub_mgr);
			m_comp_sizer->Insert(m_components.size(), panel);
			m_components.push_back(panel);
		}
		break;
	case COMP_SCISSOR:
		{
#ifndef GAME_OBJ_ECS
			auto& comp = m_owp.GetNode()->AddUniqueComp<n2::CompScissor>();
			comp.SetRect(sm::rect(100, 100));
#else
			auto& comp = m_world.GetComponent<e2::CompScissor>(m_owp);
			comp.rect = sm::rect(100, 100);
#endif // GAME_OBJ_ECS
			auto panel = new ee2::WxCompScissorPanel(this, comp, m_sub_mgr);
			m_comp_sizer->Insert(m_components.size(), panel);
			m_components.push_back(panel);

			m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
		}
		break;
	case COMP_SCRIPT:
		{
			auto panel = new ee2::WxCompScriptPanel(
#ifndef GAME_OBJ_ECS
				this, m_sub_mgr, m_owp.GetNode()
#else
				this, m_sub_mgr, m_world, m_owp
#endif // GAME_OBJ_ECS
			);
			m_comp_sizer->Insert(m_components.size(), panel);
			m_components.push_back(panel);
		}
		break;
	case COMP_CIRCLE:
	{
		auto& shape = std::make_shared<gs::Circle>();
		shape->SetRadius(50);
		auto& comp = m_owp.GetNode()->AddSharedComp<n2::CompShape>(shape);
		auto panel = new ee2::WxCompShapePanel(this, comp);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
		break;
	case COMP_RECT:
	{
		auto& shape = std::make_shared<gs::Rect>();
		shape->SetRect(sm::rect(100, 100));
		auto& comp = m_owp.GetNode()->AddSharedComp<n2::CompShape>(shape);
		auto panel = new ee2::WxCompShapePanel(this, comp);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
		break;
	case COMP_CUSTOM_PROPERTIES:
		{
			auto node = m_owp.GetNode();
			if (!node->HasUniqueComp<ee0::CompCustomProp>()) {
				node->AddUniqueComp<ee0::CompCustomProp>();
			}
			auto& cprop = node->GetUniqueComp<ee0::CompCustomProp>();
			auto panel = new ee0::WxCompCustomPropPanel(this, cprop);
			m_comp_sizer->Insert(m_components.size(), panel);
			m_components.push_back(panel);
		}
		break;
	}

	Layout();
}

}