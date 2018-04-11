#include "particle3d/WxEmitterPanel.h"
#include "particle3d/WxPropertyPanel.h"
#include "particle3d/WxComponentPanel.h"
#include "particle3d/Languages.h"

#include <ee0/WxLibraryPanel.h>
#include <ee0/WxLibraryItem.h>

#include <ps_3d.h>
#include <guard/check.h>
#include <node0/CompAsset.h>
#include <node2/CompParticle3d.h>
#include <node2/CompParticle3dInst.h>
#include <ns/GameObjFactory.h>
#include <emitter/P3dTemplate.h>
#include <emitter/P3dInstance.h>
#include <sx/StringHelper.h>

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/scrolwin.h>

namespace
{

class WxPropertyScrolled : public wxScrolledWindow
{
public:
	WxPropertyScrolled(wxWindow* parent, n2::CompParticle3d& cp3d, 
		               n2::CompParticle3dInst& cp3d_inst)
		: wxScrolledWindow(parent)
	{
		SetScrollbars(0, 1, 0, 10, 0, 0);

		auto sizer = new wxBoxSizer(wxVERTICAL);
		m_prop_panel = new eone::particle3d::WxPropertyPanel(
			this, cp3d.GetP3DTemp(), cp3d_inst.GetP3dInst());
		sizer->Add(m_prop_panel, 1, wxEXPAND);
		SetSizer(sizer);
	}

public:
	eone::particle3d::WxPropertyPanel* m_prop_panel;

}; // WxPropertyScrolled

class WxChildrenScrolled : public wxScrolledWindow
{
public:
	WxChildrenScrolled(wxWindow* parent, n2::CompParticle3d& cp3d, 
		               n2::CompParticle3dInst& cp3d_inst,
		               eone::particle3d::WxEmitterPanel* et_panel)
		: wxScrolledWindow(parent)
	{
		SetScrollbars(0, 1, 0, 10, 0, 0);

		auto sizer = new wxBoxSizer(wxVERTICAL);
		m_children_panel = new eone::particle3d::WxEmitterPanel::WxChildrenPanel(
			this, cp3d, cp3d_inst, et_panel);
		sizer->Add(m_children_panel, 1, wxEXPAND);
		SetSizer(sizer);
	}

public:
	eone::particle3d::WxEmitterPanel::WxChildrenPanel* m_children_panel;

}; // WxChildrenScrolled

}

namespace eone
{
namespace particle3d
{

WxEmitterPanel::WxEmitterPanel(wxWindow* parent, ee0::WxLibraryPanel* library,
	                           n2::CompParticle3d& cp3d,
	                           n2::CompParticle3dInst& cp3d_inst)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1, 999))
{
	InitLayout(cp3d, cp3d_inst);
	InitEmitter();

	SetDropTarget(new WxDropTarget(library, m_children_panel));
}

void WxEmitterPanel::InitLayout(n2::CompParticle3d& cp3d, n2::CompParticle3dInst& cp3d_inst)
{
	wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

	auto prop = new WxPropertyScrolled(this, cp3d, cp3d_inst);
	sizer->Add(prop, 1, wxEXPAND);
	m_main_panel = prop->m_prop_panel;

	auto children = new WxChildrenScrolled(this, cp3d, cp3d_inst, this);
	sizer->Add(children, 1, wxEXPAND);
	m_children_panel = children->m_children_panel;
	
	SetSizer(sizer);
}

void WxEmitterPanel::InitEmitter()
{
	m_main_panel->StoreToEmitter();
}

//////////////////////////////////////////////////////////////////////////
// class WxEmitterPanel::WxChildrenPanel
//////////////////////////////////////////////////////////////////////////

WxEmitterPanel::WxChildrenPanel::
WxChildrenPanel(wxWindow* parent, n2::CompParticle3d& cp3d,
	            n2::CompParticle3dInst& cp3d_inst,
	            WxEmitterPanel* et_panel)
	: wxPanel(parent)
	, m_cp3d(cp3d)
	, m_cp3d_inst(cp3d_inst)
	, m_et_panel(et_panel)
{
	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->AddSpacer(10);
	// Remove All
	{
		wxButton* btn = new wxButton(this, wxID_ANY, LANG[LK_REMOVE_ALL]);
		Connect(btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, 
			wxCommandEventHandler(WxEmitterPanel::WxChildrenPanel::OnRemoveAll));
		sizer->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);
	}
	sizer->AddSpacer(20);
	// Components
	{
		m_children_sizer = new wxBoxSizer(wxVERTICAL);
		sizer->Add(m_children_sizer);
	}
	SetSizer(sizer);
}

void WxEmitterPanel::WxChildrenPanel::
AddChild(const n0::CompAssetPtr& casset,
	         const std::string& filepath)
{
	auto sym = m_cp3d.GetP3DTemp().AddSymbol(casset);
	auto child = new WxComponentPanel(this, sym, filepath, this);
	m_children_sizer->Insert(m_children.size(), child);
	m_children_sizer->AddSpacer(10);
	m_children.push_back(child);
	Layout();
}

void WxEmitterPanel::WxChildrenPanel::
RemoveChild(WxComponentPanel* child)
{
	if (m_children.empty()) {
		return;
	}

	m_cp3d_inst.GetP3dInst().Clear();

	int idx = -1;
	for (int i = 0, n = m_children.size(); i < n; ++i) {
		if (m_children[i] == child) {
			idx = i;
			break;
		}
	}
	if (idx == -1) {
		return;
	}

	m_children_sizer->Detach(m_children[idx]);
	delete m_children[idx];
	m_children.erase(m_children.begin() + idx);

	m_cp3d.GetP3DTemp().RemoveSymbol(idx);

	Layout();

	auto cfg = m_cp3d.GetP3DTemp().GetCfg();
	GD_ASSERT(cfg->sym_count == m_children.size(), "err count");
	for (int i = 0; i < cfg->sym_count; ++i) {
		m_children[i]->SetSymbol(&cfg->syms[i]);
	}
}

void WxEmitterPanel::WxChildrenPanel::
OnRemoveAll(wxCommandEvent& event)
{
	if (m_children.empty()) {
		return;
	}

	m_cp3d_inst.GetP3dInst().Clear();

	for (int i = 0, n = m_children.size(); i < n; ++i) 
	{
		m_children_sizer->Detach(m_children[i]);
		delete m_children[i];
	}
	m_children.clear();

	m_cp3d.GetP3DTemp().RemoveAllSymbols();

	Layout();
}

//////////////////////////////////////////////////////////////////////////
// class WxEmitterPanel::WxDropTarget
//////////////////////////////////////////////////////////////////////////

WxEmitterPanel::WxDropTarget::
WxDropTarget(ee0::WxLibraryPanel* library, WxChildrenPanel* components)
	: m_library(library)
	, m_components(components)
{
}

void WxEmitterPanel::WxDropTarget::
OnDropText(wxCoord x, wxCoord y, const wxString& text)
{
	std::vector<std::string> keys;
	sx::StringHelper::Split(text.ToStdString().c_str(), ",", keys);
	if (keys.size() <= 1) {
		return;
	}

	for (int i = 1, n = keys.size(); i < n; ++i)
	{
		int idx = std::stoi(keys[i].c_str());
		auto item = m_library->GetItem(idx);
		if (!item) {
			continue;
		}
		auto casset = ns::GameObjFactory::CreateAssetComp(item->GetFilepath());
		if (!casset) {
			continue;
		}

		m_components->AddChild(casset, item->GetFilepath());
	}
}

}
}