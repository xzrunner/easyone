#include "mesh/EditPointsMeshView.h"

#ifdef MODULE_MESH

#include <node2/CompMesh.h>
#include <painting2/Mesh.h>
#include <polymesh2/PointsMesh.h>
#include <geoshape/Polygon.h>
#include <geoshape/Point2D.h>

namespace
{

class Polygon : public gs::Polygon
{
public:
	Polygon(pm2::PointsMesh& mesh)
		: gs::Polygon(mesh.GetOuterLine())
		, m_mesh(mesh)
	{
	}

	void OnChanged(eone::mesh::EditPointsMeshView::Mode mode)
	{
		m_mesh.SetOuterLine(GetVertices());
	}

private:
	pm2::PointsMesh& m_mesh;

}; // Polygon

class Point : public gs::Point2D
{
public:
	Point(pm2::PointsMesh& mesh, int idx, eone::mesh::EditPointsMeshView::Mode mode)
		: m_mesh(mesh)
		, m_idx(idx)
	{
		switch (mode)
		{
		case eone::mesh::EditPointsMeshView::Mode::BUILD:
			SetPos(mesh.GetInnerPoints()[idx]);
			break;
		case eone::mesh::EditPointsMeshView::Mode::MODIFY:
			SetPos(mesh.GetMeshData()->vertices[idx].xy);
			break;
		}
	}

	void OnChanged(eone::mesh::EditPointsMeshView::Mode mode)
	{
		switch (mode)
		{
		case eone::mesh::EditPointsMeshView::Mode::BUILD:
		{
			auto verts = m_mesh.GetInnerPoints();
			verts[m_idx] = GetPos();
			m_mesh.SetInnerPoints(verts);
		}
			break;
		case eone::mesh::EditPointsMeshView::Mode::MODIFY:
		{
			m_mesh.SetVertexPos(m_idx, GetPos());
		}
			break;
		}
	}

	int GetIndex() const { return m_idx; }

private:
	pm2::PointsMesh& m_mesh;
	int m_idx = -1;

}; // Point

}

namespace eone
{
namespace mesh
{

EditPointsMeshView::EditPointsMeshView(n2::CompMesh& cmesh)
	: m_cmesh(cmesh)
{
}

void EditPointsMeshView::Traverse(std::function<bool(const std::shared_ptr<gs::Shape2D>&)> func) const
{
	auto& mesh = m_cmesh.GetMesh();
	if (!mesh) {
		return;
	}

	auto& pm_mesh = mesh->GetMesh();
	if (!pm_mesh) {
		return;
	}

	auto pmesh = static_cast<pm2::PointsMesh*>(pm_mesh.get());
	if (!func(std::make_shared<Polygon>(*pmesh))) {
		return;
	}

	switch (m_mode)
	{
	case Mode::BUILD:
	{
		auto& points = pmesh->GetInnerPoints();
		for (int i = 0, n = points.size(); i < n; ++i) {
			if (!func(std::make_shared<Point>(*pmesh, i, m_mode))) {
				break;
			}
		}
	}
		break;
	case Mode::MODIFY:
	{
		auto& tris = pmesh->GetMeshData();
		auto& points = tris->vertices;
		for (int i = 0; i < tris->vert_num; ++i) {
			if (!func(std::make_shared<Point>(*pmesh, i, m_mode))) {
				break;
			}
		}
	}
		break;
	}
}

void EditPointsMeshView::Insert(const std::shared_ptr<gs::Shape2D>& shape)
{
	if (m_mode != Mode::BUILD) {
		return;
	}

	auto& mesh = m_cmesh.GetMesh();
	if (!mesh) {
		return;
	}
	auto& pm_mesh = mesh->GetMesh();
	if (pm_mesh)
	{
		if (shape->get_type() == rttr::type::get<gs::Point2D>())
		{
			auto points_mesh = static_cast<pm2::PointsMesh*>(pm_mesh.get());
			auto points = points_mesh->GetInnerPoints();
			points.push_back(std::static_pointer_cast<gs::Point2D>(shape)->GetPos());
			points_mesh->SetInnerPoints(points);
		}
	}
	else
	{
		if (shape->get_type() == rttr::type::get<gs::Polygon>())
		{
			std::vector<sm::vec2> points;
			auto& verts = std::static_pointer_cast<gs::Polygon>(shape)->GetVertices();
			auto pm_mesh = std::make_unique<pm2::PointsMesh>(verts, points, mesh->GetWidth(), mesh->GetHeight());
			mesh->SetMesh(std::move(pm_mesh));
		}
	}
}

void EditPointsMeshView::Delete(const std::shared_ptr<gs::Shape2D>& shape)
{
	if (m_mode != Mode::BUILD) {
		return;
	}

	auto& mesh = m_cmesh.GetMesh();
	if (!mesh) {
		return;
	}
	auto& pm_mesh = mesh->GetMesh();
	if (!pm_mesh) {
		return;
	}

	if (shape->get_type() == rttr::type::get<gs::Polygon>())
	{
		mesh->SetMesh(nullptr);
	}
	else if (shape->get_type() == rttr::type::get<gs::Point2D>())
	{
		auto point = std::static_pointer_cast<Point>(shape);
		auto idx = point->GetIndex();

		auto pmesh = static_cast<pm2::PointsMesh*>(pm_mesh.get());
		auto verts = pmesh->GetInnerPoints();
		verts.erase(verts.begin() + idx);
		pmesh->SetInnerPoints(verts);
	}
}

void EditPointsMeshView::Clear()
{
	if (m_mode != Mode::BUILD) {
		return;
	}

	auto& mesh = m_cmesh.GetMesh();
	if (mesh) {
		mesh->SetMesh(nullptr);
	}
}

void EditPointsMeshView::AddSelected(const std::shared_ptr<gs::Shape2D>& shape)
{
}

void EditPointsMeshView::ClearSelectionSet()
{
}

void EditPointsMeshView::SetCanvasDirty()
{
}

void EditPointsMeshView::ShapeChanged(const std::shared_ptr<gs::Shape2D>& shape)
{
	if (shape->get_type() == rttr::type::get<gs::Polygon>()) {
		std::static_pointer_cast<Polygon>(shape)->OnChanged(m_mode);
	} else if (shape->get_type() == rttr::type::get<gs::Point2D>()) {
		std::static_pointer_cast<Point>(shape)->OnChanged(m_mode);
	}
}

}
}

#endif // MODULE_MESH