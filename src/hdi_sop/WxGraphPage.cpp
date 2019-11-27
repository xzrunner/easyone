#include "hdi_sop/WxGraphPage.h"

#ifdef MODULE_HDI_SOP

#include "frame/Blackboard.h"
#include "frame/WxStageSubPanel.h"
//
//#include <ee0/SubjectMgr.h>
//#include <ee0/EditPanelImpl.h>
//#include <ee0/GameObj.h>
//#include <ee0/WxStageCanvas.h>
//#include <ee0/MsgHelper.h>
#include <blueprint/Blueprint.h>
#include <blueprint/MessageID.h>
//#include <blueprint/NodeHelper.h>
//#include <blueprint/NSCompNode.h>
//#include <blueprint/Node.h>
//#include <blueprint/CompNode.h>
//#include <sopview/Evaluator.h>
#include <sopview/MessageID.h>
//#include <sopview/Node.h>
//#include <sopview/SceneTree.h>
//#include <sopview/Serializer.h>
//#include <sopview/RegistNodes.h>
#include <sopview/WxToolbarPanel.h>
#include <sopview/WxStageCanvas.h>
#include <sopview/WxGeoProperty.h>
//
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
//#include <node2/CompBoundingBox.h>
//#include <node2/AABBSystem.h>
//#include <sx/ResFileHelper.h>
//#include <js/RapidJsonHelper.h>
//#include <ns/CompFactory.h>

namespace eone
{
namespace hdi_sop
{

const std::string WxGraphPage::PAGE_TYPE = "hdi_sop";

WxGraphPage::WxGraphPage(wxWindow* parent, const ee0::GameObj& obj)
    : eone::WxStagePage(parent, obj, 0)
    , m_sop_page(parent, m_sub_mgr, obj)
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

    m_messages.push_back(sopv::MSG_CLEAR_NODE_DISPLAY_TAG);
    m_messages.push_back(sopv::MSG_SCENE_ROOT_TO_NEXT_LEVEL);
    m_messages.push_back(sopv::MSG_SCENE_ROOT_SEEK_TO_PREV_LEVEL);

    RegisterAllMessages();

    InitToolbarPanel();
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
    m_sop_page.SetPreviewCanvas(canvas);

    if (canvas)
    {
        assert(m_toolbar);
        auto prop_view = m_toolbar->GetGeoPropView();
        prop_view->SetPreviewCanvas(canvas);

        auto sop_canvas = std::static_pointer_cast<sopv::WxStageCanvas>(canvas);
        sop_canvas->SetPropView(prop_view);
        sop_canvas->SetGraphStage(this);
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
    m_toolbar = static_cast<sopv::WxToolbarPanel*>(toolbar_panel->AddPagePanel([&](wxPanel* parent)->wxPanel* {
        return new sopv::WxToolbarPanel(toolbar_panel, this, m_sop_page.GetSceneTree());
    }, wxVERTICAL));
}

}
}

#endif // MODULE_HDI_SOP