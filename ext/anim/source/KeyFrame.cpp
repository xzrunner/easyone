#include "anim/KeyFrame.h"

namespace anim
{

KeyFrame::KeyFrame(int frame_idx)
	: m_frame_idx(frame_idx)
	, m_tween(false)
{
}

void KeyFrame::AddNode(const std::shared_ptr<n0::SceneNode>& node)
{
	m_nodes.push_back(node);
}

bool KeyFrame::RemoveNode(const std::shared_ptr<n0::SceneNode>& node)
{
	for (auto itr = m_nodes.begin(); itr != m_nodes.end(); ++itr)
	{
		if (*itr == node)
		{
			m_nodes.erase(itr);
			return true;
		}
	}
	return false;
}

bool KeyFrame::RemoveAllNodes()
{
	if (!m_nodes.empty()) {
		m_nodes.clear();
		return true;
	} else {
		return false;
	}
}

void KeyFrame::SetNodes(const std::vector<n0::SceneNodePtr>& nodes)
{
	m_nodes = nodes;
}

}