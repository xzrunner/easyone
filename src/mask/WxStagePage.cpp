#include "mask/WxStagePage.h"

namespace eone
{
namespace mask
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library)
	: ee0::WxStagePage(parent)
{
	m_sub_mgr.RegisterObserver(ee0::MSG_INSERT_SCENE_NODE, this);
	m_sub_mgr.RegisterObserver(ee0::MSG_CLEAR_SCENE_NODE, this);

//	SetDropTarget(new WxStageDropTarget(library, this));
}

void WxStagePage::OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants)
{
}

void WxStagePage::Traverse(std::function<bool(const n0::SceneNodePtr&)> func) const
{
	func(m_base);
	func(m_mask);
}

void WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{

}

void WxStagePage::ClearSceneNode()
{

}

}
}