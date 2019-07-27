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
#include <blueprint/CommentaryNodeHelper.h>
#include <blueprint/Node.h>
#include <blueprint/CompNode.h>
#include <intention/Evaluator.h>
#include <intention/Everything.h>

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
    , m_eval(std::make_shared<itt::Evaluator>())
{
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
		dirty = ClearSceneObj();
		break;

    case bp::MSG_BP_CONN_INSERT:
        dirty = AfterInsertNodeConn(variants);
        break;
    case bp::MSG_BP_CONN_DELETE:
        dirty = BeforeDeleteNodeConn(variants);
        break;
    case bp::MSG_BP_CONN_REBUILD:
        m_eval->OnRebuildConnection();
        dirty = true;
        break;
    case bp::MSG_BP_NODE_PROP_CHANGED:
        dirty = UpdateNodeProp(variants);
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

void WxGraphPage::LoadFromJson(const rapidjson::Value& val, const std::string& dir)
{
    m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
    m_sub_mgr->NotifyObservers(ee0::MSG_SCENE_NODE_CLEAR);

    n0::CompAssetPtr casset = ns::CompFactory::Instance()->CreateAsset(val, dir);
    if (casset)
    {
        if (m_obj->HasSharedComp<n0::CompAsset>()) {
            m_obj->RemoveSharedComp<n0::CompAsset>();
        }
        m_obj->AddSharedCompNoCreate<n0::CompAsset>(casset);
    }
    else
    {
        casset = m_obj->GetSharedCompPtr<n0::CompAsset>();
    }
    // FIXME: reinsert, for send insert msg to other panel
    if (m_obj->HasSharedComp<n0::CompComplex>())
    {
        auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
        auto nodes = ccomplex.GetAllChildren();
        ccomplex.RemoveAllChildren();
        for (auto& node : nodes) {
            ee0::MsgHelper::InsertNode(*m_sub_mgr, node, false);
        }
    }

    // copy from
    // LoadFromFileExt(filepath);
    bp::CommentaryNodeHelper::InsertNodeToCommentary(*this);
    auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
    bp::NSCompNode::LoadConnection(ccomplex.GetAllChildren(), val["nodes"]);
    m_sub_mgr->NotifyObservers(bp::MSG_BP_CONN_REBUILD);

	if (m_obj->HasUniqueComp<n2::CompBoundingBox>())
	{
		auto& cbb = m_obj->GetUniqueComp<n2::CompBoundingBox>();
		auto aabb = n2::AABBSystem::Instance()->GetBounding(*m_obj);
		cbb.SetSize(*m_obj, aabb);
	}

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxGraphPage::OnPageInit()
{
    InitToolbarPanel();
}

const n0::NodeComp& WxGraphPage::GetEditedObjComp() const
{
    return m_obj->GetSharedComp<n0::CompComplex>();
}

void WxGraphPage::StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
	                             rapidjson::MemoryPoolAllocator<>& alloc) const
{
	// connection
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	bp::NSCompNode::StoreConnection(ccomplex.GetAllChildren(), val["nodes"], alloc);

	val.AddMember("page_type", rapidjson::Value(PAGE_TYPE.c_str(), alloc), alloc);
}

//void WxGraphPage::LoadFromFileExt(const std::string& filepath)
//{
//    bp::CommentaryNodeHelper::InsertNodeToCommentary(*this);
//
//    if (sx::ResFileHelper::Type(filepath) == sx::RES_FILE_JSON)
//    {
//        rapidjson::Document doc;
//        js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);
//
//        auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
//        bp::NSCompNode::LoadConnection(ccomplex.GetAllChildren(), doc["nodes"]);
//
//        m_sub_mgr->NotifyObservers(bp::MSG_BLUE_PRINT_CHANGED);
//    }
//}

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

    auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
    ccomplex.AddChild(*obj);

    if ((*obj)->HasUniqueComp<bp::CompNode>()) {
        auto& bp_node = (*obj)->GetUniqueComp<bp::CompNode>().GetNode();
        m_eval->OnAddNode(*bp_node);
    }

    return true;
}

bool WxGraphPage::DeleteSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	bool dirty = ccomplex.RemoveChild(*obj);

    if (dirty && (*obj)->HasUniqueComp<bp::CompNode>()) {
        auto& bp_node = (*obj)->GetUniqueComp<bp::CompNode>().GetNode();
        m_eval->OnRemoveNode(*bp_node);
    }

    return dirty;
}

bool WxGraphPage::ClearSceneObj()
{
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	bool dirty = !ccomplex.GetAllChildren().empty();
	ccomplex.RemoveAllChildren();

    m_eval->OnClearAllNodes();

	return dirty;
}

bool WxGraphPage::AfterInsertNodeConn(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("conn");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: conn");
    const std::shared_ptr<bp::Connecting>* conn = static_cast<const std::shared_ptr<bp::Connecting>*>(var.m_val.pv);
    GD_ASSERT(conn, "err conn");

    m_eval->OnConnected(**conn);

    return true;
}

bool WxGraphPage::BeforeDeleteNodeConn(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("conn");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: conn");
    const std::shared_ptr<bp::Connecting>* conn = static_cast<const std::shared_ptr<bp::Connecting>*>(var.m_val.pv);
    GD_ASSERT(conn, "err conn");

    m_eval->OnDisconnecting(**conn);

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
        m_eval->OnNodePropChanged(bp_node);
        return true;
    }
    else
    {
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