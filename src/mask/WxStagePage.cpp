#include "mask/WxStagePage.h"

#include <node2/CompMask.h>

namespace eone
{
namespace mask
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library,
	                     n2::CompMask* cmask)
	: ee0::WxStagePage(parent)
	, m_cmask(cmask)
	, m_comp_new(cmask == nullptr)
{
	if (!m_cmask) {
		m_cmask = new n2::CompMask();
	}

	m_sub_mgr.RegisterObserver(ee0::MSG_INSERT_SCENE_NODE, this);
	m_sub_mgr.RegisterObserver(ee0::MSG_CLEAR_SCENE_NODE, this);

//	SetDropTarget(new WxStageDropTarget(library, this));
}

WxStagePage::~WxStagePage()
{
	if (m_comp_new) {
		delete m_cmask;
	}
}

void WxStagePage::OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants)
{
}

void WxStagePage::Traverse(std::function<bool(const n0::SceneNodePtr&)> func) const
{
	m_cmask->Traverse(func);
}

void WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{

}

void WxStagePage::ClearSceneNode()
{

}

}
}