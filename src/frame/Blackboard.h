#pragma once

#include <cu/cu_macro.h>

#include <memory>

class wxFrame;

namespace eone
{

class Application;

class Blackboard
{
public:
	void SetFrame(wxFrame* frame) { m_frame = frame; }
	wxFrame* GetFrame() { return m_frame; }

	void SetApp(const std::shared_ptr<Application>& app) { m_app = app; }
	const std::shared_ptr<Application>& GetApp() const { return m_app; }

private:
	wxFrame* m_frame;

	std::shared_ptr<Application> m_app = nullptr;

	CU_SINGLETON_DECLARATION(Blackboard);

}; // Blackboard

}