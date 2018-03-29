#pragma once

#include <node0/typedef.h>

#include <vector>

namespace anim
{

class KeyFrame
{
public:
	KeyFrame(int frame_idx);
	KeyFrame(const KeyFrame& frame);
	KeyFrame& operator = (const KeyFrame& frame);

	int  GetFrameIdx() const { return m_frame_idx; }
	void SetFrameIdx(int frame_idx) { m_frame_idx = frame_idx; }

	bool GetTween() const { return m_tween; }
	void SetTween(bool tween) { m_tween = tween; }

	void AddNode(const std::shared_ptr<n0::SceneNode>& node);
	bool RemoveNode(const std::shared_ptr<n0::SceneNode>& node);
	bool RemoveAllNodes();

	const std::vector<n0::SceneNodePtr>& GetAllNodes() const { return m_nodes; }
	void SetNodes(const std::vector<n0::SceneNodePtr>& nodes);
	
private:
	int m_frame_idx;

	std::vector<n0::SceneNodePtr> m_nodes;

	bool m_tween;
	
}; // KeyFrame

}