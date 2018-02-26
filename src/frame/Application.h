#pragma once

#include <ee0/Application.h>

#include <wx/aui/framemanager.h>

#include <memory>

class wxFrame;
class wxWindow;
class wxGLContext;

namespace ee0 { class WxLibraryPanel; }

namespace eone
{

class WxStagePanel;

class WxSceneTreePanel;

class Application : public ee0::Application
{
public:
	Application(wxFrame* frame);
	virtual ~Application();

	virtual void LoadFromFile(const std::string& filepath) override;
	virtual void StoreToFile(const std::string& filepath) const override;
	virtual void Clear() override;

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

	std::shared_ptr<wxGLContext> m_gl_ctx = nullptr;
	WxStagePanel* m_stage;

	WxSceneTreePanel* m_tree;

	friend class WxFrame;

}; // Application

}