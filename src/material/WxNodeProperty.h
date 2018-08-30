#pragma once

#include <ee0/typedef.h>

#include <blueprint/Node.h>

#include <wx/panel.h>

class wxPropertyGrid;
class wxPropertyGridEvent;

namespace eone
{
namespace material
{

class WxNodeProperty : public wxPanel
{
public:
	WxNodeProperty(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr);

	void LoadFromNode(const std::shared_ptr<bp::node::Node>& node);

private:
	void InitLayout();

	void OnPropertyGridChange(wxPropertyGridEvent& event);

private:
	ee0::SubjectMgrPtr m_sub_mgr;

	wxPropertyGrid* m_pg;

	std::shared_ptr<bp::node::Node> m_node = nullptr;

}; // WxNodeProperty

}
}