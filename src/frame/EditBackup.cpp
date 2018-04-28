#include "frame/EditBackup.h"

#include <ee0/SubjectMgr.h>
#include <ee0/AtomicOP.h>

#include <guard/check.h>

#include <boost/filesystem.hpp>

#include <fstream>

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

EditBackup::EditBackup(const ee0::SubjectMgrPtr& sub_mgr)
	: m_sub_mgr(sub_mgr)
{
	RegisterMsg();
}

void EditBackup::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
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

void EditBackup::SetFilepath(const std::string& filepath)
{
	if (m_filepath == filepath) {
		return;
	}

	// copy
	if (boost::filesystem::exists(m_filepath))
	{
		std::ifstream fin(m_filepath);
		std::ofstream fout(filepath);
		GD_ASSERT(!fin.fail() && !fout.fail(), "err file");

		std::string line;
		while (std::getline(fin, line)) {
			fout << line << '\n';
		}

		fin.close();
		fout.close();
	}

	m_filepath = filepath;
}

void EditBackup::RegisterMsg()
{
	for (auto& msg : MESSAGES) {
		m_sub_mgr->RegisterObserver(msg, this);
	}
}

void EditBackup::OnAddAOP(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("aop");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: aop");
	GD_ASSERT(var.m_val.pv, "err aop");
	auto& aop = *static_cast<std::shared_ptr<ee0::AtomicOP>*>(var.m_val.pv);

	std::locale::global(std::locale(""));
	std::ofstream fout(m_filepath, std::ios_base::app);
	std::locale::global(std::locale("C"));
	if (fout.fail()) {
		return;
	}

	fout << aop->ToScript() << std::endl;

	fout.close();
}

void EditBackup::OnEditOpUndo()
{
}

void EditBackup::OnEditOpRedo()
{
}

void EditBackup::Clear() const
{
	boost::filesystem::remove(m_filepath);
}

}