#include "anim/AnimInstance.h"

#include <ee0/CompNodeEditor.h>

#include <node0/SceneNode.h>
#include <node2/UpdateSystem.h>
#include <node2/CompTransform.h>
#include <node2/CompColorCommon.h>

namespace anim
{

AnimInstance::AnimInstance(const std::shared_ptr<const AnimTemplate>& anim_temp)
	: m_template(anim_temp)
	, m_curr_num(0)
{
	Refresh();
}

AnimInstance::AnimInstance(const AnimInstance& inst)
	: m_template(inst.m_template)
	, m_ctrl(inst.m_ctrl)
	, m_layer_cursor(inst.m_layer_cursor)
	, m_layer_cursor_update(inst.m_layer_cursor_update)
	, m_curr(inst.m_curr)
	, m_curr_num(inst.m_curr_num)
{
	m_slots.reserve(m_template->m_slots.size());
	for (auto& node : m_template->m_slots) {
		m_slots.push_back(node->Clone());
	}

	UpdateSlotsVisible();
}

AnimInstance& AnimInstance::operator = (const AnimInstance& inst)
{
	if (&inst == this) {
		return *this;
	}

	m_template = inst.m_template;

	m_ctrl = inst.m_ctrl;

	m_layer_cursor = inst.m_layer_cursor;
	m_layer_cursor_update = inst.m_layer_cursor_update;

	m_slots.clear();
	m_slots.reserve(m_template->m_slots.size());
	for (auto& node : m_template->m_slots) {
		m_slots.push_back(node->Clone());
	}

	m_curr = inst.m_curr;
	m_curr_num = inst.m_curr_num;

	UpdateSlotsVisible();

	return *this;
}

void AnimInstance::Refresh()
{
	ResetLayerCursor();

	m_slots.reserve(m_template->m_slots.size());
	for (auto& node : m_template->m_slots) {
		m_slots.push_back(node->Clone());
	}

	m_curr.clear();
	int max_num = m_template->GetMaxItemNum();
	if (max_num != m_curr.size()) {
		m_curr.resize(max_num);
	}
	m_curr_num = 0;

	UpdateSlotsVisible();
}

bool AnimInstance::Update(bool loop, float interval, int fps)
{
	if (!m_ctrl.IsActive()) {
		return UpdateChildren();
	}

	if (!m_ctrl.Update(fps)) {
		return false;
	}

	bool dirty = UpdateFrameCursor(loop, interval, fps, true);

	// update curr frame
	if (dirty) {
		LoadCurrSprites();
	}

	if (UpdateChildren()) {
		dirty = true;
	}

	return dirty;
}

bool AnimInstance::SetFrame(int frame_idx, int fps)
{
	if (frame_idx == m_ctrl.GetFrame()) {
		return false;
	}

	int frame_copy = frame_idx;
	frame_idx = frame_idx % (m_template->GetMaxFrameIdx() + 1);

	if (frame_idx < m_ctrl.GetFrame()) {
		ResetLayerCursor();
	}

	m_ctrl.SetFrame(frame_idx, fps);

	LoadCurrSprites();

	SetChildrenFrame(frame_copy, fps);

	return true;
}

void AnimInstance::TraverseCurrNodes(std::function<bool(const n0::SceneNodePtr&)> func) const
{
	if (m_curr.empty()) {
		return;
	}

	const int* curr = &m_curr[0];
	for (int i = 0; i < m_curr_num; ++i, ++curr) 
	{
		auto& child = m_slots[*curr];
		if (!func(child)) {
			break;
		}
	}
}

void AnimInstance::LoadSprLerpData(const n0::SceneNodePtr& node, const AnimTemplate::Lerp& lerp, int time)
{
	pt2::SRT srt = lerp.srt + lerp.dsrt * static_cast<float>(time);
	srt.Update();

	auto& ctrans = node->GetUniqueComp<n2::CompTransform>();
	ctrans.SetSRT(*node, srt);

	if (node->HasUniqueComp<n2::CompColorCommon>())
	{
		pt2::Color mul(lerp.col_mul), add(lerp.col_add);
		mul.r += static_cast<uint8_t>(lerp.dcol_mul[0] * time);
		mul.g += static_cast<uint8_t>(lerp.dcol_mul[1] * time);
		mul.b += static_cast<uint8_t>(lerp.dcol_mul[2] * time);
		mul.a += static_cast<uint8_t>(lerp.dcol_mul[3] * time);
		add.r += static_cast<uint8_t>(lerp.dcol_add[0] * time);
		add.g += static_cast<uint8_t>(lerp.dcol_add[1] * time);
		add.b += static_cast<uint8_t>(lerp.dcol_add[2] * time);
		add.a += static_cast<uint8_t>(lerp.dcol_add[3] * time);

		auto& ccolor = node->GetUniqueComp<n2::CompColorCommon>();
		ccolor.SetColor(pt2::RenderColorCommon(mul, add));
	}
}

bool AnimInstance::UpdateFrameCursor(bool loop, float interval, int fps, bool reset_cursor)
{
	bool update = false;
	int curr_frame = m_ctrl.GetFrame();
	int max_frame = m_template->GetMaxFrameIdx();
	int loop_max_frame = static_cast<int>(max_frame + interval * fps);
	if (loop) 
	{
		if (curr_frame <= max_frame) 
		{
			update = true;
		} 
		else if (curr_frame > max_frame && curr_frame <= loop_max_frame) 
		{
			curr_frame = 0;
			m_ctrl.SetFrame(0, fps);
			update = true;
			if (reset_cursor) {
				ResetLayerCursor();
			}
		} 
		else 
		{
			curr_frame = 0;
			m_ctrl.SetFrame(0, fps);
			update = true;
			if (reset_cursor) {
				ResetLayerCursor();
			}
		}
	} 
	else 
	{
		if (curr_frame > max_frame) {
			curr_frame = max_frame;
		}
	}
	//if (curr_frame != m_ctrl.GetFrame()) {
	//	m_ctrl.SetFrame(curr_frame, fps);
	//	update = true;
	//}
	return update;
}

void AnimInstance::ResetLayerCursor()
{
	if (m_template)
	{
		m_layer_cursor.assign(m_template->m_layers.size(), 0);
		m_layer_cursor_update.assign(m_template->m_layers.size(), false);
	}
}

void AnimInstance::LoadCurrSprites()
{
	if (m_template->GetMaxItemNum() < 0) {
		return;
	}

	UpdateCursor();
	LoadCurrSpritesImpl();
}

void AnimInstance::UpdateCursor()
{
	if (m_layer_cursor.empty()) {
		return;
	}

	assert(m_layer_cursor.size() == m_layer_cursor_update.size());
	
	int frame = m_ctrl.GetFrame();
	int* layer_cursor_ptr = &m_layer_cursor[0];
	int* layer_cursor_update_ptr = &m_layer_cursor_update[0];
	const AnimTemplate::Layer* layer_ptr = &m_template->m_layers[0];
	for (int i = 0, n = m_layer_cursor.size(); i < n; ++i, ++layer_cursor_ptr, ++layer_cursor_update_ptr, ++layer_ptr)
	{
		*layer_cursor_update_ptr = false;

		const int frame_num = layer_ptr->frames.size();
		if (frame_num == 0) {
			continue;
		}

		int cursor = *layer_cursor_ptr;
		if (cursor == INT_MAX) {
			continue;
		}

		assert(cursor < frame_num && frame_num > 0);
		const AnimTemplate::Frame* first_frame_ptr = &layer_ptr->frames[0];
		if (cursor >= 0 && cursor < frame_num && (first_frame_ptr + cursor)->frame_idx == frame + 1) {
			*layer_cursor_update_ptr = true;
		} else {
			while (frame_num > 1 && cursor < frame_num - 1 && (first_frame_ptr + cursor + 1)->frame_idx <= frame + 1) {
				++cursor;
				*layer_cursor_update_ptr = true;
			}
		}
		if (cursor == 0 && frame + 1 < (first_frame_ptr + cursor)->frame_idx) {
			cursor = -1;
		}
		if (cursor == frame_num - 1 && frame + 1 > (first_frame_ptr + cursor)->frame_idx) {
			cursor = INT_MAX;
		}

		//// unload last frame
		//int last_cursor = m_layer_cursor[i];
		//if (last_cursor != cursor && last_cursor != -1 && last_cursor != INT_MAX && cursor != INT_MAX)
		//{
		//	auto& frame = m_template->m_layers[i].frames[last_cursor];
		//	for (auto& item : frame.items) {
		//		if (item.lerp == -1) {
		//			auto& child = m_slots[item.slot];
		//			if (child->GetSymbol()->Type() == SYM_AUDIO) {
		//				up_child.SetActor(child->QueryActor(parent));
		//				child->OnMessage(up_child, MSG_STOP);
		//			}
		//		}
		//	}
		//}

		*layer_cursor_ptr = cursor;
	}
}

void AnimInstance::LoadCurrSpritesImpl()
{
	if (m_layer_cursor.empty()) {
		return;
	}

	int ctrl_frame = m_ctrl.GetFrame();

	m_curr_num = 0;
	int* layer_cursor_ptr = &m_layer_cursor[0];
	int* layer_cursor_update_ptr = &m_layer_cursor_update[0];
	for (int i = 0, n = m_layer_cursor.size(); i < n; ++i, ++layer_cursor_ptr, ++layer_cursor_update_ptr)
	{
		int cursor = *layer_cursor_ptr;
		if (cursor == -1 || cursor == INT_MAX) {
			continue;
		}
		const AnimTemplate::Layer& layer = m_template->m_layers[i];
		const AnimTemplate::Frame& frame = layer.frames[cursor];
		if (frame.items.empty()) {
			continue;
		}
		const AnimTemplate::Item* actor_ptr = &frame.items[0];
		for (int i = 0, n = frame.items.size(); i < n; ++i, ++actor_ptr)
		{
			const AnimTemplate::Item& actor = *actor_ptr;
			m_curr[m_curr_num++] = actor.slot;
			if (actor.next != -1) 
			{
				assert(actor.lerp != -1);
				const AnimTemplate::Frame& next_frame = layer.frames[cursor + 1];

				if (actor.slot != next_frame.items[actor.next].slot) {
					//int sym_id = 0;
					//if (spr) {
					//	sym_id = spr->GetSymbol()->GetID();
					//}
//					fault("anim lerp err: sym_id %d, frame %d\n", sym_id, frame);
					return;
				}

				assert(actor.slot == next_frame.items[actor.next].slot);
				auto& tween = m_slots[actor.slot];
				int time = ctrl_frame + 1 - frame.frame_idx;
				int tot_time = next_frame.frame_idx - frame.frame_idx;
				const AnimTemplate::Lerp& lerp = m_template->m_lerps[actor.lerp];
				LoadSprLerpData(tween, lerp, time);

				//auto& begin = actor.node;
				//auto& end = next_frame.items[actor.next].node;
				//AnimLerp::LerpSpecial(*begin, *end, tween, time, tot_time);

				//AnimLerp::LerpExpression(*begin, *end, tween, time, tot_time, frame.lerps);
			}
			else if (actor.prev != -1)
			{
				assert(actor.lerp == -1);
				const AnimTemplate::Frame& pre_frame = layer.frames[cursor - 1];
				const AnimTemplate::Item& pre_actor = pre_frame.items[actor.prev];
				assert(actor.slot == pre_actor.slot);
				auto& tween = m_slots[pre_actor.slot];
				int time = frame.frame_idx - pre_frame.frame_idx;
				const AnimTemplate::Lerp& lerp = m_template->m_lerps[pre_actor.lerp];
				LoadSprLerpData(tween, lerp, time);

				//AnimLerp::LerpSpecial(*pre_actor.node, *actor.node, tween, time, time);
			}
			else
			{
				auto& src = m_template->m_slots[actor.slot];
				auto& srt = src->GetUniqueComp<n2::CompTransform>().GetTrans().GetSRT();
				auto& dst = m_slots[actor.slot];
				dst->GetUniqueComp<n2::CompTransform>().SetSRT(*dst, srt);
			}

			//bool last_frame = cursor == layer.frames.size() - 1;
			//if (!last_frame && *layer_cursor_update_ptr && actor.prev == -1)
			//{
			//	SprPtr& child = m_slots[actor.slot];
			//	up_child.SetActor(child->QueryActor(up.GetActor()));
			//	child->OnMessage(up_child, MSG_TRIGGER);
			//}
			//// MSG_TRIGGER for the last frame
			//if (*layer_cursor_update_ptr && actor.prev == -1)
			//{
			//	SprPtr& child = m_slots[actor.slot];
			//	up_child.SetActor(child->QueryActor(parent));
			//	child->OnMessage(up_child, MSG_TRIGGER);
			//}
		}
	}

	UpdateSlotsVisible();
}

bool AnimInstance::UpdateChildren()
{
	if (m_curr.empty()) {
		return false;
	}

	bool dirty = false;
	int* curr = &m_curr[0];
	for (int i = 0; i < m_curr_num; ++i, ++curr)
	{
		auto& child = m_slots[*curr];
		if (n2::UpdateSystem::Update(child)) {
			dirty = true;
		}
	}
	return dirty;
}

void AnimInstance::SetChildrenFrame(int frame, int fps)
{
	//if (m_layer_cursor.empty()) {
	//	return;
	//}

	//int* layer_cursor_ptr = &m_layer_cursor[0];
	//for (int i = 0, n = m_layer_cursor.size(); i < n; ++i, ++layer_cursor_ptr)
	//{
	//	int cursor = *layer_cursor_ptr;
	//	if (cursor == -1 || cursor == INT_MAX) {
	//		continue;
	//	}
	//	const AnimTemplate::Layer& layer = m_template->m_layers[i];
	//	const AnimTemplate::Frame& frame = layer.frames[cursor];
	//	for (int j = 0, m = frame.items.size(); j < m; ++j)
	//	{
	//		// find first time
	//		int first_time = frame.frame_idx;
	//		int frame_idx = cursor, actor_idx = j;
	//		while (frame_idx >= 0 && layer.frames[frame_idx].items[actor_idx].prev != -1) {
	//			actor_idx = layer.frames[frame_idx].items[actor_idx].prev;
	//			--frame_idx;
	//			first_time = layer.frames[frame_idx].frame_idx;
	//		}

	//		const AnimTemplate::Item& actor = frame.items[j];
	//		auto& node = m_slots[actor.slot];

 //			SetStaticFrameVisitor visitor(static_frame - first_time + 1);
	//		SprVisitorParams vp;
	//		vp.actor = child->QueryActor(up.GetActor());
	//		child->Traverse(visitor, vp, false);
	//	}
	//}
}

void AnimInstance::UpdateSlotsVisible()
{
	if (!m_slots.empty()) 
	{
		for (auto& node : m_slots) {
			auto& ceditor = node->GetUniqueComp<ee0::CompNodeEditor>();
			ceditor.SetVisible(false);
		}
	}

	if (!m_curr.empty()) 
	{
		int* curr = &m_curr[0];
		for (int i = 0; i < m_curr_num; ++i, ++curr) {
			auto& ceditor = m_slots[*curr]->GetUniqueComp<ee0::CompNodeEditor>();
			ceditor.SetVisible(true);
		}
	}
}

}