#pragma once

#include "frame/config.h"

#ifdef MODULE_MESH

#include <node0/typedef.h>

#include <drawing2/EditView.h>

namespace n2 { class CompMesh; }

namespace eone
{
namespace mesh
{

class EditPointsMeshView : public dw2::EditView
{
public:
	enum class Mode
	{
		BUILD,
		MODIFY,
	};

public:
	EditPointsMeshView(n2::CompMesh& cmesh);

	// node
	virtual void Traverse(std::function<bool(const std::shared_ptr<gs::Shape2D>&)> func) const override;
	virtual void Insert(const std::shared_ptr<gs::Shape2D>& shape) override;
	virtual void Delete(const std::shared_ptr<gs::Shape2D>& shape) override;
	virtual void Clear() override;

	// selection
	virtual void AddSelected(const std::shared_ptr<gs::Shape2D>& shape) override;
	virtual void ClearSelectionSet() override;

	// canvas
	virtual void SetCanvasDirty() override;

	// shape
	virtual void ShapeChanged(const std::shared_ptr<gs::Shape2D>& shape) override;

	Mode GetMode() const { return m_mode; }
	void SetMode(Mode mode) { m_mode = mode; }

private:
	n2::CompMesh& m_cmesh;

	Mode m_mode = Mode::BUILD;

}; // EditPointsMeshView

}
}

#endif // MODULE_MESH