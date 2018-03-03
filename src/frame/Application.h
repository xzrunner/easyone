#pragma once

#include <ee0/Application.h>
#include <ee0/RenderContext.h>
#include <ee0/WindowContext.h>

#include <wx/aui/framemanager.h>

#include <memory>

class wxFrame;
class wxWindow;

namespace ee0 { class WxLibraryPanel; }
namespace gum { class RenderContext; }

namespace eone
{

class WxStagePanel;
class WxPreviewPanel;

class WxSceneTreePanel;

class Application : public ee0::Application
{
public:
	Application(wxFrame* frame);
	virtual ~Application();

	virtual void LoadFromFile(const std::string& filepath) override;
	virtual void StoreToFile(const std::string& filepath) const override;
	virtual void Clear() override;

	ee0::WxLibraryPanel* GetLiraryPanel() { return m_library; }
	WxStagePanel* GetStagePanel() { return m_stage; }

	const ee0::RenderContext& GetRenderContext() const { return m_rc; }
	const ee0::WindowContext& GetWindowContext() const { return m_wc; }

private:
	void InitSubmodule();
	void InitLayout();
	void InitCallback();

	wxWindow* CreateLibraryPanel();
	wxWindow* CreateStagePanel();
	wxWindow* CreatePreviewPanel();
	wxWindow* CreateTreePanel();
	wxWindow* CreateDetailPanel();

private:
	wxFrame* m_frame;

	wxAuiManager m_mgr;

	ee0::WxLibraryPanel* m_library;

	ee0::RenderContext m_rc;
	ee0::WindowContext m_wc;

	WxStagePanel* m_stage;
	WxPreviewPanel* m_preview;

	WxSceneTreePanel* m_tree;

}; // Application

}