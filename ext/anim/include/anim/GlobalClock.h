#pragma once

#include <cu/cu_macro.h>

namespace anim
{

class GlobalClock
{
public:
	void Update(float dt);

	float GetTime() const { return m_time; }

private:
	float m_time;

	CU_SINGLETON_DECLARATION(GlobalClock)

}; // GlobalClock

}