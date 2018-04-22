#pragma once

#include <ee0/WxImageVList.h>
#include <ee0/typedef.h>

namespace eone
{

class WxItemsListList : public ee0::WxImageVList
{
public:
	WxItemsListList(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr);

	virtual void OnListSelected(wxCommandEvent& event) override;

private:
	ee0::SubjectMgrPtr m_sub_mgr;

}; // WxItemsListList

}