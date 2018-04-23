#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <wx/panel.h>

class wxListBox;

namespace eone
{

class WxRecordPanel : public wxPanel, public ee0::Observer
{
public:
	WxRecordPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

private:
	void InitLayout();
	void RegisterMsg();

	void OnAddAOP(const ee0::VariantSet& variants);
	void OnEditOpUndo();
	void OnEditOpRedo();

private:
	ee0::SubjectMgrPtr m_sub_mgr;

	wxListBox* m_list;

}; // WxRecordPanel

}