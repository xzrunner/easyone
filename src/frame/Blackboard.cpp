#include "frame/Blackboard.h"

namespace eone
{

CU_SINGLETON_DEFINITION(Blackboard);

Blackboard::Blackboard()
	: m_frame(nullptr)
	, m_stage(nullptr)
{
}

}