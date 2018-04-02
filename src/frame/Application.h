#pragma once

#include <ee0/Application.h>

#include <wx/aui/framemanager.h>

#include <memory>

class wxFrame;
class wxWindow;
class wxGLCanvas;

namespace eone
{

class WxStagePanel;

class Application : public ee0::Application
{
public:
	Application(wxFrame* frame);
	virtual ~Application();

	virtual void LoadFromFile(const std::string& filepath) override;
	virtual void StoreToFile(const std::string& filepath) const override;
	virtual void Clear() override;

	WxStagePanel* GetStagePanel() { return m_stage; }

	wxAuiManager& GetUIManager() { return m_mgr; }

private:
	void InitSubmodule();
	void InitLayout();
	void InitCallback();

	wxWindow* CreateLibraryPanel();
	wxWindow* CreateStagePanel();
	wxWindow* CreateStageExtPanel();
	wxWindow* CreatePreviewPanel();
	wxWindow* CreateTreePanel();
	wxWindow* CreateDetailPanel();
	wxWindow* CreateToolbarPanel();

private:
	wxFrame* m_frame;

	wxAuiManager m_mgr;

	WxStagePanel* m_stage;

}; // Application

}