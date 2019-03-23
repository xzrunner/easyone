#include "sgraph/WxStageCanvas.h"

#include <ee0/WxStagePage.h>
#include <ee0/MsgHelper.h>
#include <ee0/SubjectMgr.h>
#include <blueprint/CompNode.h>
#include <blueprint/Node.h>
#include <blueprint/MessageID.h>
#include <blueprint/NodeHelper.h>
#include <blueprint/node/SetReference.h>
#include <blueprint/node/GetReference.h>
#include <blueprint/node/SetValue.h>
#include <blueprint/node/GetValue.h>
#include <shadergraph/RegistNodes.h>
#include <shadergraph/TypeDeduction.h>

#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>

namespace eone
{
namespace sgraph
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage, const ee0::RenderContext& rc)
	: WxStageCanvas2D(stage, rc)
{
}

bool WxStageCanvas::OnUpdate()
{
	WxStageCanvas2D::OnUpdate();

    bool get_var_name_dirty = false;

//	bool bp_dirty = false;
	bool canvas_dirty = false;
	m_stage->Traverse([&](const ee0::GameObj& obj)->bool
	{
		if (obj->HasUniqueComp<bp::CompNode>())
		{
			auto& bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
			if (bp_node->IsLifeDeleteLater())
			{
				ee0::MsgHelper::DeleteNode(*m_stage->GetSubjectMgr(), obj);
//				bp_dirty = true;
				canvas_dirty = true;
				return false;
			}
			if (bp_node->IsSizeChanged())
			{
				bp_node->SetSizeChanged(false);
				auto& st = bp_node->GetStyle();
				sm::rect sz(st.width, st.height);
				obj->GetUniqueComp<n2::CompBoundingBox>().SetSize(*obj, sz);
			}
            if (bp_node->IsGetVarNameChanged()) {
                get_var_name_dirty = true;
            }
			if (!canvas_dirty) {
				canvas_dirty = bp::NodeHelper::HasInputNode<sg::node::Time>(*bp_node);
			}
		}
		return true;
	});

    if (get_var_name_dirty) {
        OnGetVarNameChanged();
    }

	//if (bp_dirty) {
	//	m_stage->GetSubjectMgr()->NotifyObservers(bp::MSG_BLUE_PRINT_CHANGED);
	//}
	if (canvas_dirty) {
		m_stage->GetSubjectMgr()->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	}
	return canvas_dirty;
}

void WxStageCanvas::OnGetVarNameChanged() const
{
    std::map<std::string, int> val_var2type;
    std::map<std::string, int> ref_var2type;
    m_stage->Traverse([&](const ee0::GameObj& obj)->bool
    {
        if (!obj->HasUniqueComp<bp::CompNode>()) {
            return true;
        }

        auto& bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
        auto bp_type = bp_node->get_type();
        if (bp_type == rttr::type::get<bp::node::SetValue>()) {
            auto val = std::static_pointer_cast<bp::node::SetValue>(bp_node);
            val_var2type.insert({ val->GetName(), val->GetAllInput()[0]->GetType() });
        } else if (bp_type == rttr::type::get<bp::node::SetReference>()) {
            auto ref = std::static_pointer_cast<bp::node::SetReference>(bp_node);
            ref_var2type.insert({ ref->GetName(), ref->GetAllInput()[0]->GetType() });
        }
        return true;
    });

    m_stage->Traverse([&](const ee0::GameObj& obj)->bool
    {
        if (!obj->HasUniqueComp<bp::CompNode>()) {
            return true;
        }

        auto& bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
        if (!bp_node->IsGetVarNameChanged()) {
            return true;
        }

        int type = -1;
        auto bp_type = bp_node->get_type();
        if (bp_type == rttr::type::get<bp::node::GetValue>())
        {
            auto val = std::static_pointer_cast<bp::node::SetValue>(bp_node);
            auto itr = val_var2type.find(val->GetName());
            if (itr != val_var2type.end()) {
                type = itr->second;
            }
        }
        else if (bp_type == rttr::type::get<bp::node::GetReference>())
        {
            auto ref = std::static_pointer_cast<bp::node::SetReference>(bp_node);
            auto itr = ref_var2type.find(ref->GetName());
            if (itr != ref_var2type.end()) {
                type = itr->second;
            }
        }

        if (type != -1)
        {
            assert(bp_node->GetAllOutput().size() == 1);
            auto& port = bp_node->GetAllOutput()[0];
            port->SetType(type);
            port->SetOldType(type);

            for (auto& c : port->GetConnecting()) {
                sg::TypeDeduction::DeduceConn(*port, *c->GetTo());
            }
        }

        return true;
    });
}

}
}