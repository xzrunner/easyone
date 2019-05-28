#include "guigraph/WxGraphPage.h"

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

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node2/CompBoundingBox.h>
#include <node2/AABBSystem.h>
#include <sx/ResFileHelper.h>
#include <js/RapidJsonHelper.h>
#include <ns/CompFactory.h>

namespace eone
{
namespace guigraph
{

const std::string WxGraphPage::PAGE_TYPE = "gui_graph";

WxGraphPage::WxGraphPage(wxWindow* parent, const ee0::GameObj& obj)
    : eone::WxStagePage(parent, obj, 0)
{
    static bool inited = false;
    if (!inited) {
        inited = true;
        bp::Blueprint::Instance();
    }

	m_messages.push_back(ee0::MSG_SCENE_NODE_INSERT);
	m_messages.push_back(ee0::MSG_SCENE_NODE_DELETE);
	m_messages.push_back(ee0::MSG_SCENE_NODE_CLEAR);

	m_messages.push_back(bp::MSG_BLUE_PRINT_CHANGED);

    RegisterAllMessages();
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

    case bp::MSG_BLUE_PRINT_CHANGED:
        UpdateBlueprint();
        break;
	}

	if (dirty) {
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
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

    // LoadFromFileExt(filepath);
    auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
    bp::NSCompNode::LoadConnection(ccomplex.GetAllChildren(), val["nodes"]);

	if (m_obj->HasUniqueComp<n2::CompBoundingBox>())
	{
		auto& cbb = m_obj->GetUniqueComp<n2::CompBoundingBox>();
		auto aabb = n2::AABBSystem::Instance()->GetBounding(*m_obj);
		cbb.SetSize(*m_obj, aabb);
	}

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
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

void WxGraphPage::LoadFromFileExt(const std::string& filepath)
{
    bp::CommentaryNodeHelper::InsertNodeToCommentary(*this);

    if (sx::ResFileHelper::Type(filepath) == sx::RES_FILE_JSON)
    {
        rapidjson::Document doc;
        js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);

        auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
        bp::NSCompNode::LoadConnection(ccomplex.GetAllChildren(), doc["nodes"]);

        m_sub_mgr->NotifyObservers(bp::MSG_BLUE_PRINT_CHANGED);
    }
}

bool WxGraphPage::InsertSceneObj(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("obj");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
    GD_ASSERT(obj, "err scene obj");

    auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
    ccomplex.AddChild(*obj);

    return true;
}

bool WxGraphPage::DeleteSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	return ccomplex.RemoveChild(*obj);
}

bool WxGraphPage::ClearSceneObj()
{
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	bool dirty = !ccomplex.GetAllChildren().empty();
	ccomplex.RemoveAllChildren();
	return dirty;
}

void WxGraphPage::UpdateBlueprint()
{
}

bool WxGraphPage::UpdateNodes()
{
    bool dirty = false;
    auto& wc = GetImpl().GetCanvas()->GetWidnowContext();
    bp::UpdateParams params(wc.wc2, wc.wc3);
    Traverse([&](const ee0::GameObj& obj)->bool
    {
        if (!obj->HasUniqueComp<bp::CompNode>()) {
            return true;
        }
        auto& bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
        if (bp_node->Update(params)) {
            dirty = true;
        }
        return true;
    });
    return dirty;
}

}
}