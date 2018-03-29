#include "anim/PlayCtrl.h"
#include "anim/GlobalClock.h"

#include <cmath>

#include <assert.h>

#include <stdio.h>

namespace anim
{

PlayCtrl::PlayCtrl()
	: m_frame(-1)
	, m_start_time(0)
	, m_curr_time(0)
	, m_stop_time(0)
	, m_stop_during(0)
	, m_active(true)
{
}

void PlayCtrl::SetActive(bool active)
{
	if (m_active == active) {
		return;
	}

	if (active) 
	{
		float cost = m_curr_time - m_start_time - m_stop_during;
		if (m_stop_time > 0) {
			m_stop_during += GlobalClock::Instance()->GetTime() - m_stop_time;
			m_stop_time = 0;
		}
		m_curr_time = GlobalClock::Instance()->GetTime();
		m_start_time = m_curr_time - m_stop_during - cost;
	} 
	else 
	{
		m_stop_time = GlobalClock::Instance()->GetTime();
	}
	m_active = active; 
}

bool PlayCtrl::SetFrame(int frame, int fps)
{
	bool ret = m_frame != frame;

	m_frame = frame;

	m_curr_time = GlobalClock::Instance()->GetTime();
	m_start_time = m_curr_time - static_cast<float>(m_frame) / fps;

	m_stop_time = -1;
	m_stop_during = 0;

	return ret;
}

bool PlayCtrl::Update(int fps)
{
	if (!m_active) {
		return false;
	}

	float curr_time = GlobalClock::Instance()->GetTime();
	if (curr_time == m_curr_time) {
		return false;
	}

	m_curr_time = curr_time;

	int frame = std::lround((m_curr_time - m_start_time - m_stop_during) * fps);
	bool ret = frame != m_frame;
	m_frame = frame;

	return ret;
}

}