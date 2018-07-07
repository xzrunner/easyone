#include "scene3d/QuakeMapLoader.h"

#include <ee0/MsgHelper.h>

#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <quake/MapParser.h>
#include <quake/WadFileLoader.h>
#include <quake/Palette.h>
#include <quake/TextureManager.h>
#include <model/EffectType.h>
#include <model/MeshGeometry.h>
#include <model/typedef.h>
#include <model/Model.h>
#include <node0/SceneNode.h>
#include <node0/CompIdentity.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <node3/CompTransform.h>
#include <node3/CompAABB.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/filesystem.hpp>

#include <fstream>

namespace
{

struct Vertex
{
	sm::vec3 pos;
	sm::vec2 texcoord;
};

const sm::vec3 X_AXIS(1, 0, 0);
const sm::vec3 Y_AXIS(0, 0, -1);

void CreateMeshRenderBuf(model::Model::Mesh& mesh, const std::vector<Vertex>& vertices)
{
	const int stride = sizeof(Vertex) / sizeof(float);

	ur::RenderContext::VertexInfo vi;

	vi.vn = vertices.size();
	vi.vertices = &vertices[0].pos.x;
	vi.stride = sizeof(Vertex);

	vi.in = 0;
	vi.indices = nullptr;

	vi.va_list.push_back(ur::VertexAttrib("pos", 3, 4, 20, 0));	// pos
	vi.va_list.push_back(ur::VertexAttrib("texcoord", 2, 4, 20, 12));	// texcoord

	ur::Blackboard::Instance()->GetRenderContext().CreateVAO(
		vi, mesh.geometry.vao, mesh.geometry.vbo, mesh.geometry.ebo);
	mesh.geometry.sub_geometries.push_back(model::SubmeshGeometry(false, vi.vn, 0));
	mesh.geometry.sub_geometry_materials.push_back(0);
	mesh.geometry.vertex_type |= model::VERTEX_FLAG_TEXCOORDS;
}

}

namespace eone
{

void QuakeMapLoader::LoadFromFile(ee0::SubjectMgr& sub_mgr,
	                              const std::string& filepath)
{
	std::ifstream fin(filepath);
	std::string str((std::istreambuf_iterator<char>(fin)),
		std::istreambuf_iterator<char>());
	fin.close();

	quake::MapParser parser(str);
	parser.Parse();

	auto dir = boost::filesystem::path(filepath).parent_path().string();

	// load textures
	auto world = parser.GetWorldEntity();
	assert(world);
	LoadTextures(*world, dir);

	// load models
	std::vector<std::shared_ptr<model::Model>> models;
	LoadEntities(parser.GetAllEntities(), models);

	// insert models
	for (auto& model : models)
	{
		auto node = std::make_shared<n0::SceneNode>();

		// model
		auto& cmodel = node->AddSharedComp<n3::CompModel>();
		cmodel.SetModel(model);
		cmodel.SetFilepath(filepath);
		node->AddUniqueComp<n3::CompModelInst>(model, 0);

		// transform
		auto& ctrans = node->AddUniqueComp<n3::CompTransform>();

		// aabb
		node->AddUniqueComp<n3::CompAABB>(pt3::AABB(model->aabb));

		// id
		node->AddUniqueComp<n0::CompIdentity>();

		ee0::MsgHelper::InsertNode(sub_mgr, node);
	}
}

void QuakeMapLoader::LoadTextures(const quake::MapEntity& world,
	                              const std::string& dir)
{
	std::string tex_path;
	for (auto& attr : world.attributes)
	{
		if (attr.name == "wad") {
			tex_path = attr.val;
			break;
		}
	}

	assert(!tex_path.empty());
	std::vector<std::string> paths;
	boost::split(paths, tex_path, boost::is_any_of(";"));

	quake::Palette palette;
	quake::WadFileLoader loader(palette);
	for (auto& path : paths)
	{
		if (path.empty()) {
			continue;
		}
		auto full_path = boost::filesystem::absolute(path, dir);
		loader.Load(full_path.string());
	}
}

void QuakeMapLoader::LoadEntities(const std::vector<std::unique_ptr<quake::MapEntity>>& entities,
	                              std::vector<std::shared_ptr<model::Model>>& models)
{
	auto tex_mgr = quake::TextureManager::Instance();
	int count = 0;
	for (auto& e : entities)
	{
		auto model = std::make_unique<model::Model>();
		for (auto& b : e->brushes)
		{
			if (b.faces.empty()) {
				continue;
			}

			// sort by texture
			auto faces = b.faces;
			std::sort(faces.begin(), faces.end(), [](const quake::MapFace& lhs, const quake::MapFace& rhs) {
				return lhs.tex_name < rhs.tex_name;
			});

			// create meshes
			std::unique_ptr<model::Model::Mesh> mesh = nullptr;
			std::vector<Vertex> vertices;
			std::string curr_tex_name;
			ur::TexturePtr curr_tex = nullptr;
			for (auto& f : faces)
			{
				// new material
				if (f.tex_name != curr_tex_name)
				{
					if (!vertices.empty()) {
						CreateMeshRenderBuf(*mesh, vertices);
						model->meshes.push_back(std::move(mesh));
						vertices.clear();
					}

					mesh = std::make_unique<model::Model::Mesh>();
					mesh->effect = model::EFFECT_USER;

					auto mat = std::make_unique<model::Model::Material>();
					int mat_idx = model->materials.size();
					mesh->material = mat_idx;
					auto tex = tex_mgr->Query(f.tex_name);
					mat->diffuse_tex = model->textures.size();
					model->textures.push_back({ f.tex_name, tex });
					model->materials.push_back(std::move(mat));

					curr_tex_name = f.tex_name;
					curr_tex = tex;
				}

				for (int i = 0; i < 3; ++i)
				{
					Vertex v;

					v.pos = f.vertices[i];

					auto& s = f.scale;
					v.texcoord.x = (v.pos.Dot(X_AXIS / s.x) + f.offset.x) / curr_tex->Width();
					v.texcoord.y = (v.pos.Dot(Y_AXIS / s.y) + f.offset.y) / curr_tex->Height();

					vertices.push_back(v);
				}
			}

			if (!vertices.empty()) {
				CreateMeshRenderBuf(*mesh, vertices);
				model->meshes.push_back(std::move(mesh));
				vertices.clear();
			}
		}
		models.push_back(std::move(model));
	}
}

}