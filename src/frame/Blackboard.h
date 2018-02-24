#pragma once

#include <cu/cu_macro.h>

class wxFrame;
class wxWindow;

namespace eone
{

class Blackboard
{
public:
	void SetFrame(wxFrame* frame) { m_frame = frame; }
	wxFrame* GetFrame() { return m_frame; }

	void SetStage(wxWindow* stage) { m_stage = stage; }
	wxWindow* GetStage() { return m_stage; }

private:
	wxFrame* m_frame;

	wxWindow* m_stage;

	CU_SINGLETON_DECLARATION(Blackboard);

}; // Blackboard

}