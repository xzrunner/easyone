#pragma once

#include <ee0/WxEditPanel.h>
#include <ee0/Observer.h>

#include <node0/typedef.h>

namespace eone
{

class WxStagePage;

class WxPreviewPanel : public ee0::WxEditPanel, public ee0::Observer
{
public:
	WxPreviewPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr, 
		WxStagePage* stage);

	virtual void OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants) override;

	const ee0::SubjectMgrPtr& GetSubjectMgr() const { return m_sub_mgr; }

	WxStagePage& GetStagePage() { return *m_stage; }

private:
	void RegisterMsg(ee0::SubjectMgr& sub_mgr);

	void StagePageChanged(const ee0::VariantSet& variants);

private:
	ee0::SubjectMgrPtr m_sub_mgr;

	WxStagePage* m_stage;

}; // WxPreviewPanel

}