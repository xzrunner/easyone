#pragma once

#include "anim/PlayCtrl.h"
#include "anim/AnimTemplate.h"

#include <node0/typedef.h>

#include <memory>
#include <vector>
#include <functional>

namespace anim
{

class AnimInstance
{
public:
	AnimInstance(const std::shared_ptr<const AnimTemplate>& anim_temp);
	AnimInstance(const AnimInstance& inst);
	AnimInstance& operator = (const AnimInstance& inst);

	void Refresh();
	
	bool Update(bool loop = true, float interval = 0, int fps = 30);
	bool SetFrame(int frame_idx, int fps);

	void TraverseCurrNodes(std::function<bool(const n0::SceneNodePtr&)> func) const;

	const std::shared_ptr<const AnimTemplate>& GetAnimTemplate() const {
		return m_template;
	}

	PlayCtrl& GetPlayCtrl() { return m_ctrl; }
	const PlayCtrl& GetPlayCtrl() const { return m_ctrl; }

	static void LoadSprLerpData(const n0::SceneNodePtr& node, const AnimTemplate::Lerp& lerp, int time);

private:
	bool UpdateFrameCursor(bool loop, float interval, int fps, bool reset_cursor);

	void ResetLayerCursor();

	void LoadCurrSprites();
	void UpdateCursor();
	void LoadCurrSpritesImpl();
	bool UpdateChildren();

	void SetChildrenFrame(int frame, int fps);

	void UpdateSlotsVisible();

private:
	std::shared_ptr<const AnimTemplate> m_template = nullptr;

	PlayCtrl m_ctrl;

	std::vector<int> m_layer_cursor;
	std::vector<int> m_layer_cursor_update;

	std::vector<n0::SceneNodePtr> m_slots;

	std::vector<int> m_curr;
	int              m_curr_num;

}; // AnimInstance

}