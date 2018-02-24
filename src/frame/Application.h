#pragma once

#include <wx/aui/framemanager.h>

class wxFrame;
class wxWindow;

namespace ee0 { class WxLibraryPanel; }

namespace eone
{

class WxStagePanel;

class WxSceneTreePanel;

class Application
{
public:
	Application(wxFrame* frame);
	~Application();

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

	WxStagePanel* m_stage;

	WxSceneTreePanel* m_tree;

}; // Application

}