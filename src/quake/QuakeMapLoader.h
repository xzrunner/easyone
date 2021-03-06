#pragma once

#include "frame/config.h"

#ifdef MODULE_QUAKE

#include <string>

namespace ee0 { class SubjectMgr; }

namespace eone
{
namespace quake
{

class QuakeMapLoader
{
public:
	static void LoadFromFile(ee0::SubjectMgr& sub_mgr,
		const std::string& filepath);

}; // QuakeMapLoader

}
}

#endif // MODULE_QUAKE