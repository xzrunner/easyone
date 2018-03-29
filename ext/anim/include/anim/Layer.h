#pragma once

#include "anim/typedef.h"

#include <vector>

namespace anim
{

class Layer
{
public:
	Layer() {}
	Layer(const Layer& layer);
	Layer& operator = (const Layer& layer);

	void AddKeyFrame(KeyFramePtr& frame);

	void SetName(const std::string& name);
	std::string GetName() const;

	bool IsEditable() const;
	void SetEditable(bool editable);
	bool IsVisible() const;
	void SetVisible(bool visible);

	int GetMaxFrameIdx() const;

	KeyFrame* GetCurrKeyFrame(int frame_idx) const;

	const std::vector<KeyFramePtr>& GetAllKeyFrames() { return m_frames; }

	bool IsKeyFrame(int frame_idx) const;
	 
	void InsertNullFrame(int frame_idx);
	void RemoveNullFrame(int frame_idx);

	void InsertKeyFrame(int frame_idx);
	void RemoveKeyFrame(int frame_idx);

public:
	struct EditInfo
	{
		std::string name;
		bool editable, visible;
	};

private:
	std::vector<KeyFramePtr> m_frames;

	std::unique_ptr<EditInfo> m_edit_info = nullptr;

}; // Layer

}