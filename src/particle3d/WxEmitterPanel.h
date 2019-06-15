#pragma once

#include "frame/config.h"

#ifdef MODULE_PARTICLE3D

#include <ee0/GameObj.h>
#include <ee0/WxDropTarget.h>

#include <wx/panel.h>
#include <wx/dnd.h>

#include <memory>

namespace ee0 { class WxLibraryPanel; }
namespace n2 { class CompParticle3d; class CompParticle3dInst; }

namespace eone
{
namespace particle3d
{

class WxPropertyPanel;
class WxComponentPanel;

class WxEmitterPanel : public wxPanel
{
public:
	WxEmitterPanel(wxWindow* parent, ee0::WxLibraryPanel* library,
		n2::CompParticle3d& cp3d, n2::CompParticle3dInst& cp3d_inst);

private:
	void InitLayout(n2::CompParticle3d& cp3d, n2::CompParticle3dInst& cp3d_inst);
	void InitEmitter();

public:
	class WxChildrenPanel : public wxPanel
	{
	public:
		WxChildrenPanel(wxWindow* parent, n2::CompParticle3d& cp3d,
			n2::CompParticle3dInst& cp3d_inst, WxEmitterPanel* et_panel);

		// todo ecs
#ifndef GAME_OBJ_ECS
		void AddChild(const n0::CompAssetPtr& casset,
			const std::string& filepath);
#endif // GAME_OBJ_ECS
		void RemoveChild(WxComponentPanel* child);

	private:
		void OnRemoveAll(wxCommandEvent& event);

	private:
		n2::CompParticle3d&     m_cp3d;
		n2::CompParticle3dInst& m_cp3d_inst;

		WxEmitterPanel* m_et_panel;

		wxSizer* m_children_sizer;
		std::vector<WxComponentPanel*> m_children;

	}; // WxChildrenPanel

private:
	class WxDropTarget : public ee0::WxDropTarget
	{
	public:
		WxDropTarget(ee0::WxLibraryPanel* library, WxChildrenPanel* components);

		virtual void OnDropText(wxCoord x, wxCoord y, const wxString& text) override;
		virtual void OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) override {}

	private:
		ee0::WxLibraryPanel* m_library;
		WxChildrenPanel*     m_components;

	}; // DropTarget

private:
	WxPropertyPanel* m_main_panel;
	WxChildrenPanel* m_children_panel;

}; // WxEmitterPanel

}
}

#endif // MODULE_PARTICLE3D