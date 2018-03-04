#pragma once

#include <ee0/RenderContext.h>
#include <ee0/WindowContext.h>

#include <cu/cu_macro.h>

#include <memory>

class wxFrame;
class wxGLCanvas;

namespace ee0 { class WxLibraryPanel; }

namespace eone
{

class Application;
class WxStagePanel;
class WxPreviewPanel;

class Blackboard
{
public:
	void SetFrame(wxFrame* frame) { m_frame = frame; }
	wxFrame* GetFrame() { return m_frame; }

	void SetApp(const std::shared_ptr<Application>& app) { m_app = app; }
	const std::shared_ptr<Application>& GetApp() const { return m_app; }

	void SetLiraryPanel(ee0::WxLibraryPanel* library) { m_library = library; }
	ee0::WxLibraryPanel* GetLiraryPanel() { return m_library; }

	void SetStagePanel(WxStagePanel* stage) { m_stage = stage; }
	WxStagePanel* GetStagePanel() { return m_stage; }

	void SetPreviewPanel(WxPreviewPanel* preview) { m_preview = preview; }
	WxPreviewPanel* GetPreviewPanel() { return m_preview; }

	void InitRenderContext();
	const ee0::RenderContext& GetRenderContext() const { return m_rc; }
	const ee0::WindowContext& GetWindowContext() const { return m_wc; }

private:
	wxFrame* m_frame;

	ee0::WxLibraryPanel* m_library;
	WxStagePanel*        m_stage;
	WxPreviewPanel*      m_preview;

	wxGLCanvas* m_dummy_canvas;
	ee0::RenderContext m_rc;
	ee0::WindowContext m_wc;

	std::shared_ptr<Application> m_app = nullptr;

	CU_SINGLETON_DECLARATION(Blackboard);

}; // Blackboard

}