#pragma once

#include <wx/aui/framemanager.h>

class wxFrame;
class wxWindow;

namespace ee0 { class WxLibraryPanel; }

namespace eone
{

class StagePanel;

class SceneTreeCtrl;

class Task
{
public:
	Task(wxFrame* frame);

private:
	void InitLayout();
	void InitCallback();

	wxWindow* CreateLibraryPanel();
	wxWindow* CreateStagePanel();
	wxWindow* CreateTreePanel();
	wxWindow* CreateDetailPanel();

private:
	wxFrame* m_frame;

	wxAuiManager m_mgr;

	ee0::WxLibraryPanel* m_library;

	StagePanel* m_stage;

	SceneTreeCtrl* m_tree;

}; // Task

}