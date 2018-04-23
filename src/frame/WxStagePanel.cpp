#include "frame/WxStagePanel.h"
#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"

#include <guard/check.h>
#include <ee0/MsgHelper.h>
#include <ee0/SubjectMgr.h>
#include <js/RapidJsonHelper.h>

#include <boost/filesystem.hpp>

namespace eone
{

WxStagePanel::WxStagePanel(wxWindow* parent)
	: wxAuiNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
		wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER)
{
	Connect(GetId(), wxEVT_AUINOTEBOOK_PAGE_CLOSE,
		wxAuiNotebookEventHandler(WxStagePanel::OnPageClose));
	Connect(GetId(), wxEVT_AUINOTEBOOK_PAGE_CHANGING, 
		wxAuiNotebookEventHandler(WxStagePanel::OnPageChanging));
	Connect(GetId(), wxEVT_AUINOTEBOOK_PAGE_CHANGED,
		wxAuiNotebookEventHandler(WxStagePanel::OnPageChanged));
}

WxStagePage* WxStagePanel::GetCurrentStagePage() const
{
	return static_cast<WxStagePage*>(GetCurrentPage());
}

void WxStagePanel::AddNewPage(WxStagePage* page, const std::string& title)
{
	AddPage(page, title);
	SetSelection(GetPageCount() - 1);
}

bool WxStagePanel::SwitchToPage(const std::string& filepath)
{
	for (size_t i = 0; i < GetPageCount(); ++i)
	{
		auto page = static_cast<WxStagePage*>(GetPage(i));
		if (page->GetFilepath() == filepath) {
			SetSelection(i);
			return true;
		}
	}
	return false;
}

std::string WxStagePanel::StoreCurrPage(const std::string& filepath)
{
	auto page = GetCurrentStagePage();
	if (!page) {
		return "";
	}

	std::string _filepath = filepath;
	if (_filepath.empty()) {
		_filepath = page->GetFilepath();
	}
	if (_filepath.empty())
	{
		wxFileDialog dlg(Blackboard::Instance()->GetFrame(), wxT("Save"),
			wxEmptyString, wxEmptyString, "*.json", wxFD_SAVE);
		if (dlg.ShowModal() == wxID_OK) {
			_filepath = dlg.GetPath().ToStdString();
		} else {
			return "";
		}
	}
	if (_filepath.empty()) {
		return "";
	}

	page->SetFilepath(_filepath);

	rapidjson::Document doc;
	doc.SetArray();

	auto dir = boost::filesystem::path(_filepath).parent_path().string();

	auto& alloc = doc.GetAllocator();

	page->StoreToJson(dir, doc, alloc);

	js::RapidJsonHelper::WriteToFile(_filepath.c_str(), doc);

	page->GetEditRecord().OnSave();
	ee0::MsgHelper::SetEditorDirty(*page->GetSubjectMgr(), false);

	return _filepath;
}

void WxStagePanel::OnPageClose(wxAuiNotebookEvent& event)
{
	auto page = GetCurrentStagePage();
	m_old_sub_mgr = page->GetSubjectMgr();
	if (!page->IsEditDirty()) {
		return;
	}

	int answer = wxMessageBox("Save the file?", "Close", wxYES_NO | wxCANCEL, this);
	if (answer == wxYES)
	{
		auto path = StoreCurrPage("");
		if (path.empty()) {
			event.Veto();
		}
	}
	else if (answer == wxNO)
	{
		;
	}
	else
	{
		event.Veto();
	}
}

void WxStagePanel::OnPageChanging(wxAuiNotebookEvent& event)
{
	auto page = GetCurrentStagePage();
	if (page) 
	{
		m_old_sub_mgr = page->GetSubjectMgr();
		m_old_sub_mgr->NotifyObservers(ee0::MSG_STAGE_PAGE_CHANGING);
	}
}

void WxStagePanel::OnPageChanged(wxAuiNotebookEvent& event)
{
	if (!m_old_sub_mgr) {
		return;
	}

	auto new_page = GetCurrentStagePage();
	GD_ASSERT(new_page, "null new_page");
	Blackboard::Instance()->GetFrame()->SetTitle(new_page->GetFilepath());

	m_old_sub_mgr->NotifyObservers(ee0::MSG_STAGE_PAGE_ON_HIDE);

	ee0::VariantSet vars;

	ee0::Variant var;
	var.m_type = ee0::VT_PVOID;
	var.m_val.pv = new_page;
	vars.SetVariant("new_page", var);

	m_old_sub_mgr->NotifyObservers(ee0::MSG_STAGE_PAGE_CHANGED, vars);

	new_page->GetSubjectMgr()->NotifyObservers(ee0::MSG_STAGE_PAGE_ON_SHOW);

	m_old_sub_mgr.reset();
}

}