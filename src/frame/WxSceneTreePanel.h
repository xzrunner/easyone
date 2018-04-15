#pragma once

#include <ee0/GameObj.h>
#include <ee0/typedef.h>

#include <wx/panel.h>

#ifdef GAME_OBJ_ECS
namespace ecsx { class World; }
#endif // GAME_OBJ_ECS

namespace eone
{

class WxSceneTreePanel : public wxPanel
{
public:
	WxSceneTreePanel(
		wxWindow* parent, 
		const ee0::SubjectMgrPtr& sub_mgr,
#ifdef GAME_OBJ_ECS
		ecsx::World& world,
#endif // GAME_OBJ_ECS
		const ee0::GameObj& root_obj);

private:
	void InitLayout(const ee0::GameObj& root_obj);

	void OnCreatePress(wxCommandEvent& event);

private:
	ee0::SubjectMgrPtr m_sub_mgr;
#ifdef GAME_OBJ_ECS
	ecsx::World&       m_world;
#endif // GAME_OBJ_ECS

	wxButton* m_create_btn;

}; // WxSceneTreePanel

}