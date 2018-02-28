#include "frame/WxStagePage.h"

namespace eone
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ee0::SubjectMgr& preview_sub_mgr)
	: ee2::WxStagePage(parent, library)
	, m_preview_sub_mgr(preview_sub_mgr)
{
}

void WxStagePage::OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants)
{
	ee2::WxStagePage::OnNotify(msg, variants);

	m_preview_sub_mgr.NotifyObservers(msg, variants);
}

}