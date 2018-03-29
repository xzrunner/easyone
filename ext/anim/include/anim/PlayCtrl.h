#pragma once

namespace anim
{

class PlayCtrl
{
public:
	PlayCtrl();

	bool IsActive() const { return m_active; }
	void SetActive(bool active);

	bool SetFrame(int frame, int fps);
	int  GetFrame() const { return m_frame; }

	bool Update(int fps);

private:
	int m_frame;

	float m_start_time, m_curr_time;
	float m_stop_time, m_stop_during;

	bool m_active;

}; // PlayCtrl

}