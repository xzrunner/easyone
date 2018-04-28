#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <string>

namespace eone
{

class EditBackup : public ee0::Observer
{
public:
	EditBackup(const ee0::SubjectMgrPtr& sub_mgr);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	void SetFilepath(const std::string& filepath);

	void Clear() const;

private:
	void RegisterMsg();
	
	void OnAddAOP(const ee0::VariantSet& variants);
	void OnEditOpUndo();
	void OnEditOpRedo();

private:
	ee0::SubjectMgrPtr m_sub_mgr;

	std::string m_filepath;

}; // EditBackup

}