#pragma once

#include <wx/aui/framemanager.h>

class wxFrame;
class wxWindow;

namespace ee0 { class WxLibraryPanel; }

namespace eone
{

class StagePanel;

class SceneTreePanel;

class Task
{
public:
	Task(wxFrame* frame);

private:
	void InitSubmodule();
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

	SceneTreePanel* m_tree;

}; // Task

}