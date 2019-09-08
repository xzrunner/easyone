#include "ittgraph/WxGraphPage.h"

#ifdef MODULE_ITTGRAPH

#include "ittgraph/WxToolbarPanel.h"

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
#include <intention/Evaluator.h>
#include <intention/Everything.h>
#include <intention/MessageID.h>
#include <intention/Node.h>
#include <intention/SceneTree.h>
#include <intention/Serializer.h>
#include <intention/RegistNodes.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node2/CompBoundingBox.h>
#include <node2/AABBSystem.h>
#include <sx/ResFileHelper.h>
#include <js/RapidJsonHelper.h>
#include <ns/CompFactory.h>

namespace eone
{
namespace ittgraph
{

const std::string WxGraphPage::PAGE_TYPE = "itt_graph";

WxGraphPage::WxGraphPage(wxWindow* parent, const ee0::GameObj& obj)
    : eone::WxStagePage(parent, obj, 0)
{
    m_stree = std::make_shared<itt::SceneTree>();
    if (!m_obj->HasUniqueComp<bp::CompNode>()) {
        auto& cnode = m_obj->AddUniqueComp<bp::CompNode>();
        cnode.SetNode(std::make_shared<itt::node::Geometry>());
    }
    m_stree->Add(m_obj);
    m_stree->ToNextLevel(m_obj);

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

    m_messages.push_back(itt::MSG_CLEAR_NODE_DISPLAY_TAG);
    m_messages.push_back(itt::MSG_SCENE_ROOT_TO_NEXT_LEVEL);
    m_messages.push_back(itt::MSG_SCENE_ROOT_SEEK_TO_PREV_LEVEL);

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
        dirty = m_stree->Clear();
		break;

    case bp::MSG_BP_CONN_INSERT:
        dirty = AfterInsertNodeConn(variants);
        break;
    case bp::MSG_BP_CONN_DELETE:
        dirty = BeforeDeleteNodeConn(variants);
        break;
    case bp::MSG_BP_CONN_REBUILD:
    {
        auto eval = m_stree->GetCurrEval();
        assert(eval);
        eval->OnRebuildConnection();
        dirty = true;
    }
        break;
    case bp::MSG_BP_NODE_PROP_CHANGED:
        dirty = UpdateNodeProp(variants);
        break;

    case itt::MSG_CLEAR_NODE_DISPLAY_TAG:
        m_stree->ClearNodeDisplayTag();
        dirty = true;
        break;
    case itt::MSG_SCENE_ROOT_TO_NEXT_LEVEL:
        dirty = ChangeSceneRoot(variants);
        break;
    case itt::MSG_SCENE_ROOT_SEEK_TO_PREV_LEVEL:
        dirty = PathSeekToPrev(variants);
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
    m_toolbar = static_cast<WxToolbarPanel*>(toolbar_panel->AddPagePanel([&](wxPanel* parent)->wxPanel* {
        return new WxToolbarPanel(toolbar_panel, this);
    }, wxVERTICAL));
}

bool WxGraphPage::InsertSceneObj(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("obj");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
    GD_ASSERT(obj, "err scene obj");

    return m_stree->Add(*obj);
}

bool WxGraphPage::DeleteSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

    return m_stree->Remove(*obj);
}

bool WxGraphPage::AfterInsertNodeConn(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("conn");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: conn");
    const std::shared_ptr<bp::Connecting>* conn = static_cast<const std::shared_ptr<bp::Connecting>*>(var.m_val.pv);
    GD_ASSERT(conn, "err conn");

    auto eval = m_stree->GetCurrEval();
    assert(eval);
    eval->OnConnected(**conn);

    return true;
}

bool WxGraphPage::BeforeDeleteNodeConn(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("conn");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: conn");
    const std::shared_ptr<bp::Connecting>* conn = static_cast<const std::shared_ptr<bp::Connecting>*>(var.m_val.pv);
    GD_ASSERT(conn, "err conn");

    auto eval = m_stree->GetCurrEval();
    assert(eval);
    eval->OnDisconnecting(**conn);

    return true;
}

bool WxGraphPage::UpdateNodeProp(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("obj");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
    GD_ASSERT(obj, "err scene obj");

    if ((*obj)->HasUniqueComp<bp::CompNode>())
    {
        auto& bp_node = (*obj)->GetUniqueComp<bp::CompNode>().GetNode();
        auto eval = m_stree->GetCurrEval();
        assert(eval);
        eval->OnNodePropChanged(bp_node);
        return true;
    }
    else
    {
        return false;
    }
}

bool WxGraphPage::ChangeSceneRoot(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("obj");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
    GD_ASSERT(obj, "err scene obj");

    if (m_obj == *obj) {
        return false;
    } else {
        m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
        m_obj = *obj;
        return true;
    }
}

bool WxGraphPage::PathSeekToPrev(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("depth");
    GD_ASSERT(var.m_type == ee0::VT_ULONG, "no var in vars: obj");
    auto depth = var.m_val.ul;
    if (m_stree->SetDepth(depth)) {
        m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
        m_obj = m_stree->GetCurrNode();
        return true;
    } else {
        return false;
    }
}

//bool WxGraphPage::UpdateNodes()
//{
//    bool dirty = false;
//    auto& wc = GetImpl().GetCanvas()->GetWidnowContext();
//    bp::UpdateParams params(wc.wc2, wc.wc3);
//    Traverse([&](const ee0::GameObj& obj)->bool
//    {
//        if (!obj->HasUniqueComp<bp::CompNode>()) {
//            return true;
//        }
//        auto& bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
//        if (bp_node->Update(params)) {
//            dirty = true;
//        }
//        return true;
//    });
//    return dirty;
//}

}
}

#endif // MODULE_ITTGRAPH