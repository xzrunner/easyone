#include "frame/WxRecordPanel.h"
#include "frame/config.h"

#include <ee0/SubjectMgr.h>
#include <ee0/AtomicOP.h>

#include <guard/check.h>

#include <wx/listbox.h>
#include <wx/sizer.h>

namespace
{

const uint32_t MESSAGES[] =
{
	ee0::MSG_ATOMIC_OP_ADD,
	ee0::MSG_EDIT_OP_UNDO,
	ee0::MSG_EDIT_OP_REDO,
};

}

namespace eone
{

WxRecordPanel::WxRecordPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr)
	: wxPanel(parent)
	, m_sub_mgr(sub_mgr)
{
	InitLayout();

	m_list->Append("<empty>");

	RegisterMsg();
}

void WxRecordPanel::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_ATOMIC_OP_ADD:
		OnAddAOP(variants);
		break;
	case ee0::MSG_EDIT_OP_UNDO:
		OnEditOpUndo();
		break;
	case ee0::MSG_EDIT_OP_REDO:
		OnEditOpRedo();
		break;
	}
}

void WxRecordPanel::InitLayout()
{
	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	m_list = new wxListBox(this, wxID_ANY, wxDefaultPosition,
		wxDefaultSize, 0, 0, wxLB_SINGLE);
	sizer->Add(m_list, 1, wxEXPAND);
	m_list->SetBackgroundColour(PANEL_COLOR);

	SetSizer(sizer);
}

void WxRecordPanel::RegisterMsg()
{
	for (auto& msg : MESSAGES) {
		m_sub_mgr->RegisterObserver(msg, this);
	}
}

void WxRecordPanel::OnAddAOP(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("aop");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: aop");
	GD_ASSERT(var.m_val.pv, "err aop");
	auto& aop = *static_cast<std::shared_ptr<ee0::AtomicOP>*>(var.m_val.pv);
	m_list->Append(aop->ToString());

	m_list->SetSelection(m_list->GetCount() - 1);
}

void WxRecordPanel::OnEditOpUndo()
{
	int sel = m_list->GetSelection();
	if (sel != 0) {
		m_list->SetSelection(sel - 1);
	}
}

void WxRecordPanel::OnEditOpRedo()
{
	int sel = m_list->GetSelection();
	if (sel != m_list->GetCount() - 1) {
		m_list->SetSelection(sel + 1);
	}
}

}