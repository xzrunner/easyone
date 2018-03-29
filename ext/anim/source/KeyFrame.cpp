#include "anim/KeyFrame.h"

#include <node0/SceneNode.h>

namespace anim
{

KeyFrame::KeyFrame(int frame_idx)
	: m_frame_idx(frame_idx)
	, m_tween(false)
{
}

KeyFrame::KeyFrame(const KeyFrame& frame)
	: m_frame_idx(frame.m_frame_idx)
	, m_tween(frame.m_tween)
{
	m_nodes.reserve(frame.m_nodes.size());
	for (auto& node : frame.m_nodes) {
		m_nodes.push_back(node->Clone());
	}
}

KeyFrame& KeyFrame::operator = (const KeyFrame& frame)
{
	if (&frame == this) {
		return *this;
	}

	m_frame_idx = frame.m_frame_idx;

	m_nodes.clear();
	m_nodes.reserve(frame.m_nodes.size());
	for (auto& node : frame.m_nodes) {
		m_nodes.push_back(node->Clone());
	}

	m_tween = frame.m_tween;

	return *this;
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