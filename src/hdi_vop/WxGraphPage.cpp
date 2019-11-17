#include "hdi_vop/WxGraphPage.h"

#ifdef MODULE_HDI_VOP

#include "frame/Blackboard.h"
#include "frame/WxStageSubPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee0/EditPanelImpl.h>
#include <ee0/GameObj.h>
#include <ee0/WxStageCanvas.h>
#include <ee0/MsgHelper.h>
#include <blueprint/Blueprint.h>
#include <blueprint/MessageID.h>
#include <blueprint/NodeHelper.h>
#include <blueprint/NSCompNode.h>
#include <blueprint/Node.h>
#include <blueprint/CompNode.h>
#include <vopview/Node.h>
#include <vopview/RegistNodes.h>
#include <vopview/WxToolbarPanel.h>
#include <vopview/WxStageCanvas.h>
#include <vopview/Evaluator.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node2/CompBoundingBox.h>
#include <node2/AABBSystem.h>
#include <sx/ResFileHelper.h>
#include <js/RapidJsonHelper.h>
#include <ns/CompFactory.h>

namespace eone
{
namespace hdi_vop
{

const std::string WxGraphPage::PAGE_TYPE = "vop";

WxGraphPage::WxGraphPage(wxWindow* parent, const ee0::GameObj& obj)
    : eone::WxStagePage(parent, obj, 0)
{
    m_eval = std::make_shared<vopv::Evaluator>();

    static bool inited = false;
    if (!inited) {
        inited = true;
        bp::Blueprint::Instance();
    }

	m_messages.push_back(ee0::MSG_SCENE_NODE_INSERT);
	m_messages.push_back(ee0::MSG_SCENE_NODE_DELETE);
	m_messages.push_back(ee0::MSG_SCENE_NODE_CLEAR);

    m_messages.push_back(bp::MSG_BP_CONN_INSERT);
    m_messages.push_back(bp::MSG_BP_CONN_DELETE);
    m_messages.push_back(bp::MSG_BP_CONN_REBUILD);
    m_messages.push_back(bp::MSG_BP_NODE_PROP_CHANGED);

    RegisterAllMessages();

    InitToolbarPanel();
}

void WxGraphPage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	eone::WxStagePage::OnNotify(msg, variants);

	bool dirty = false;
	switch (msg)
	{
	case ee0::MSG_SCENE_NODE_INSERT:
		dirty = InsertSceneObj(variants);
		break;
	case ee0::MSG_SCENE_NODE_DELETE:
		dirty = DeleteSceneObj(variants);
		break;
	case ee0::MSG_SCENE_NODE_CLEAR:
        dirty = ClearAllSceneObjs();
		break;
	}

	if (dirty)
    {
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
        m_preview_canvas->SetDirty();
	}
}

void WxGraphPage::Traverse(std::function<bool(const ee0::GameObj&)> func,
                           const ee0::VariantSet& variants , bool inverse) const
{
	auto var = variants.GetVariant("type");
	if (var.m_type == ee0::VT_EMPTY)
	{
		m_obj->GetSharedComp<n0::CompComplex>().Traverse(func, inverse);
		return;
	}

	GD_ASSERT(var.m_type == ee0::VT_LONG, "err type");
    switch (var.m_val.l)
    {
    case TRAV_DRAW_PREVIEW:
        func(m_obj);
        break;
        // todo ecs
    default:
        m_obj->GetSharedComp<n0::CompComplex>().Traverse(func, inverse);
    }
}

void WxGraphPage::SetPreviewCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas)
{
    m_preview_canvas = canvas;

    if (m_preview_canvas)
    {
        auto canvas = std::static_pointer_cast<vopv::WxStageCanvas>(m_preview_canvas);
        canvas->SetGraphStage(this);
    }
}

void WxGraphPage::OnPageInit()
{
    InitToolbarPanel();
}

const n0::NodeComp& WxGraphPage::GetEditedObjComp() const
{
    return m_obj->GetSharedComp<n0::CompComplex>();
}

void WxGraphPage::InitToolbarPanel()
{
    assert(!m_toolbar);
    auto toolbar_panel = Blackboard::Instance()->GetToolbarPanel();
    m_toolbar = static_cast<vopv::WxToolbarPanel*>(toolbar_panel->AddPagePanel([&](wxPanel* parent)->wxPanel* {
        return new vopv::WxToolbarPanel(toolbar_panel, this);
    }, wxVERTICAL));
}

bool WxGraphPage::ClearAllSceneObjs()
{
    // update scene node
    assert(m_obj->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
    bool dirty = !ccomplex.GetAllChildren().empty();
    ccomplex.RemoveAllChildren();

    // front eval cb
    assert(m_eval);
    m_eval->OnClearAllNodes();

    return true;
}

bool WxGraphPage::InsertSceneObj(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("obj");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
    GD_ASSERT(obj, "err scene obj");

    // update scene node
    assert(m_obj->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
    ccomplex.AddChild(*obj);

    if ((*obj)->HasUniqueComp<bp::CompNode>())
    {
        auto& bp_node = (*obj)->GetUniqueComp<bp::CompNode>().GetNode();

        // front eval cb
        m_eval->OnAddNode(*bp_node);

        //// update flags
        //auto type = bp_node->get_type();
        //if (type.is_derived_from<vopv::Node>()) {
        //    auto sopv_node = std::static_pointer_cast<vopv::Node>(bp_node);
        //    if (m_enable_set_node_display) {
        //        sopv_node->SetDisplay(true);
        //    }
        //}
    }

    return true;
}

bool WxGraphPage::DeleteSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

    // update scene (*obj)
    assert(m_obj->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
    bool dirty = ccomplex.RemoveChild(*obj);

    if (dirty && (*obj)->HasUniqueComp<bp::CompNode>()) {
        auto& bp_node = (*obj)->GetUniqueComp<bp::CompNode>().GetNode();
        m_eval->OnRemoveNode(*bp_node);
    }

    return dirty;
}

}
}

#endif // MODULE_HDI_VOP