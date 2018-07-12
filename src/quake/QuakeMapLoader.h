#pragma once

#include <string>
#include <vector>

namespace quake { struct MapEntity; }
namespace model { struct Model; }
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

private:
	static void LoadTextures(const ::quake::MapEntity& world,
		const std::string& dir);

	static void LoadEntities(const std::vector<std::unique_ptr<::quake::MapEntity>>& entities,
		std::vector<std::shared_ptr<model::Model>>& models);

}; // QuakeMapLoader

}
}