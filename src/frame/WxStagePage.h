#pragma once

#include <ee2/WxStagePage.h>

namespace eone
{

class WxStagePage : public ee2::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, 
		ee0::SubjectMgr& preview_sub_mgr);

	virtual void OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants) override;

private:
	ee0::SubjectMgr& m_preview_sub_mgr;

}; // WxStagePage

}