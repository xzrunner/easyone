#pragma once

#include <ee0/WxEditPanel.h>
#include <ee0/Observer.h>

#include <node0/typedef.h>

namespace ee0 { class SubjectMgr; }

namespace eone
{

class WxStagePage;

class WxPreviewPanel : public ee0::WxEditPanel, public ee0::Observer
{
public:
	WxPreviewPanel(wxWindow* parent, ee0::SubjectMgr& sub_mgr, 
		WxStagePage* stage);

	virtual void OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants) override;

	ee0::SubjectMgr& GetSubjectMgr() { return *m_sub_mgr; }

	WxStagePage& GetStagePage() { return *m_stage; }

private:
	void RegisterMsg(ee0::SubjectMgr& sub_mgr);

	void StagePageChanging(const ee0::VariantSet& variants);

private:
	ee0::SubjectMgr* m_sub_mgr;

	WxStagePage* m_stage;

}; // WxPreviewPanel

}