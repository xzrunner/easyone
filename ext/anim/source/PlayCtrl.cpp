#include "anim/PlayCtrl.h"
#include "anim/GlobalClock.h"

#include <assert.h>

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

	if (active) {
		m_stop_during += GlobalClock::Instance()->GetTime() - m_stop_time;
		m_stop_time = 0;
	} else {
		m_stop_time = GlobalClock::Instance()->GetTime();
	}
	m_active = active; 
}

void PlayCtrl::SetFrame(int frame)
{
	m_frame = frame;
}

void PlayCtrl::SetFrame(int frame, int fps)
{
	m_frame = frame;

	m_curr_time = GlobalClock::Instance()->GetTime();
	m_start_time = m_curr_time - static_cast<float>(m_frame) / fps;

	m_stop_time = 0;
	m_stop_during = 0;
}

void PlayCtrl::SetTime(float time)
{
	static const float dt = 0.01f;
	m_curr_time = GlobalClock::Instance()->GetTime() - dt;
	m_start_time = m_curr_time - time;
	m_stop_time = 0;
	m_stop_during = -dt;
}

bool PlayCtrl::Update()
{
	float curr_time = GlobalClock::Instance()->GetTime() - m_stop_during;
	assert(m_curr_time <= curr_time);
	if (curr_time == m_curr_time) {
		m_curr_time = curr_time;
		return false;
	} else {
		m_curr_time = curr_time;
		return true;
	}
}

void PlayCtrl::Clear()
{
	Reset();
	m_frame = -1;
}

void PlayCtrl::Reset()
{
	m_start_time = m_curr_time = GlobalClock::Instance()->GetTime();
	m_stop_time = m_stop_during = 0;
}

}