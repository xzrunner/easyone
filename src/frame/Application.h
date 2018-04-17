#pragma once

#include <ee0/Application.h>
#include <ee0/Config.h>

#ifdef GAME_OBJ_ECS
#include <ecsx/World.h>
#endif // GAME_OBJ_ECS

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

#ifdef GAME_OBJ_ECS
	ecsx::World& GetWorld() { return m_world; }
#endif // GAME_OBJ_ECS

private:
	void InitSubmodule();
	void InitLayout();
	void InitCallback();
#ifdef GAME_OBJ_ECS
	void InitWorld();
#endif // GAME_OBJ_ECS

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

#ifdef GAME_OBJ_ECS
	ecsx::World m_world;
#endif // GAME_OBJ_ECS

}; // Application

}