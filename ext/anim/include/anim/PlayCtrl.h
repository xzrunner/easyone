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

	bool Update();

	float GetStartTime() const { return m_start_time; }
	float GetCurrTime() const { return m_curr_time; }

private:
	int m_frame;

	float m_start_time, m_curr_time;
	float m_stop_time, m_stop_during;

	bool m_active;

}; // PlayCtrl

}