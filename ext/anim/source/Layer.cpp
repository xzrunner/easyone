#include "anim/Layer.h"
#include "anim/KeyFrame.h"

#include <algorithm>

namespace
{

struct FrameLessThan
{
	bool operator() (const anim::KeyFramePtr& left, const anim::KeyFramePtr& right)
	{
		return left->GetFrameIdx() < right->GetFrameIdx();
	}
	bool operator() (const anim::KeyFramePtr& left, int right)
	{
		return left->GetFrameIdx() < right;
	}
	bool operator() (int left, const anim::KeyFramePtr& right)
	{
		return left < right->GetFrameIdx();
	}
}; // FrameLessThan

}

namespace anim
{

Layer::Layer(const Layer& layer)
{
	m_frames.reserve(layer.m_frames.size());
	for (auto& frame : layer.m_frames) {
		m_frames.push_back(std::make_unique<KeyFrame>(*frame));
	}
	if (layer.m_edit_info) {
		m_edit_info = std::make_unique<EditInfo>(*layer.m_edit_info);
	}
}

Layer& Layer::operator = (const Layer& layer)
{
	if (&layer == this) {
		return *this;
	}

	m_frames.clear();
	m_frames.reserve(layer.m_frames.size());
	for (auto& frame : layer.m_frames) {
		m_frames.push_back(std::make_unique<KeyFrame>(*frame));
	}
	if (layer.m_edit_info) {
		m_edit_info = std::make_unique<EditInfo>(*layer.m_edit_info);
	}
	return *this;
}

void Layer::AddKeyFrame(KeyFramePtr& frame)
{
	m_frames.push_back(std::move(frame));
}

void Layer::SetName(const std::string& name)
{
	if (!m_edit_info) {
		m_edit_info = std::make_unique<EditInfo>();
	}
	m_edit_info->name = name;
}

std::string Layer::GetName() const
{
	return m_edit_info ? m_edit_info->name : "";
}

bool Layer::IsEditable() const
{
	return m_edit_info ? m_edit_info->editable : true;
}

void Layer::SetEditable(bool editable)
{
	if (!m_edit_info) {
		m_edit_info = std::make_unique<EditInfo>();
	}
	m_edit_info->editable = editable;
}

bool Layer::IsVisible() const
{
	return m_edit_info ? m_edit_info->visible : true;
}

void Layer::SetVisible(bool visible)
{
	if (!m_edit_info) {
		m_edit_info = std::make_unique<EditInfo>();
	}
	m_edit_info->visible = visible;
}

int Layer::GetMaxFrameIdx() const
{
	if (m_frames.empty()) {
		return -1;
	} else {
		return m_frames.back()->GetFrameIdx();
	}
}

KeyFrame* Layer::GetCurrKeyFrame(int frame_idx) const
{
	auto itr = std::lower_bound(m_frames.begin(), m_frames.end(), frame_idx, FrameLessThan());
	if (itr == m_frames.end()) {
		return nullptr;
	}
	if ((*itr)->GetFrameIdx() > frame_idx && itr != m_frames.begin()) {
		--itr;
	}
	return itr->get();
}

bool Layer::IsKeyFrame(int frame_idx) const
{
	auto itr = std::lower_bound(m_frames.begin(), m_frames.end(), frame_idx, FrameLessThan());
	return itr != m_frames.end() && (*itr)->GetFrameIdx() == frame_idx;
}

void Layer::InsertNullFrame(int frame_idx)
{
	auto itr = std::lower_bound(m_frames.begin(), m_frames.end(), frame_idx, FrameLessThan());
	for (; itr != m_frames.end(); ++itr) {
		(*itr)->SetFrameIdx((*itr)->GetFrameIdx() + 1);
	}
}

void Layer::RemoveNullFrame(int frame_idx)
{
	auto itr = std::lower_bound(m_frames.begin(), m_frames.end(), frame_idx, FrameLessThan());
	for (; itr != m_frames.end(); ++itr) {
		(*itr)->SetFrameIdx((*itr)->GetFrameIdx() - 1);
	}
}

void Layer::InsertKeyFrame(int frame_idx)
{
	auto frame = std::make_unique<KeyFrame>(frame_idx);
	m_frames.insert(std::upper_bound(m_frames.begin(), m_frames.end(),
		frame, FrameLessThan()), std::move(frame));
}

void Layer::RemoveKeyFrame(int frame_idx)
{
	auto itr = std::lower_bound(m_frames.begin(), m_frames.end(), frame_idx, FrameLessThan());
	m_frames.erase(itr);
}

}