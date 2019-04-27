#include "physics3d/WxStagePage.h"

#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"
#include "frame/AppStyle.h"
#include "frame/GameObjFactory.h"

#include <ee0/SubjectMgr.h>
#include <ee0/MsgHelper.h>
#include <ee3/WxStageDropTarget.h>
#include <ee3/WxStageCanvas.h>

#include <guard/check.h>
#include <model/Model.h>
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node3/CompModel.h>
#include <node3/CompCloth.h>
#include <sx/ResFileHelper.h>
#include <uniphysics/cloth/ClothMeshData.h>
#include <uniphysics/cloth/Solver.h>
#include <uniphysics/cloth/nv/Factory.h>
#include <unirender/VertexAttrib.h>
#include <unirender/Shader.h>
#include <unirender/Blackboard.h>
#include <facade/ResPool.h>
#include <facade/ImageCube.h>

#include <NvCloth/Factory.h>

namespace eone
{
namespace physics3d
{

const std::string WxStagePage::PAGE_TYPE = "physics3d";

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_STAGE)
{
	m_messages.push_back(ee0::MSG_SCENE_NODE_INSERT);
	m_messages.push_back(ee0::MSG_SCENE_NODE_DELETE);
	m_messages.push_back(ee0::MSG_SCENE_NODE_CLEAR);

	if (library) {
		SetDropTarget(new ee3::WxStageDropTarget(ECS_WORLD_VAR library, this));
	}

    m_factory = std::make_unique<up::cloth::nv::Factory>(nv::cloth::Platform::CPU);
}

void WxStagePage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	eone::WxStagePage::OnNotify(msg, variants);

	switch (msg)
	{
	case ee0::MSG_SCENE_NODE_INSERT:
		InsertSceneNode(variants);
		break;
	case ee0::MSG_SCENE_NODE_DELETE:
		DeleteSceneNode(variants);
		break;
	case ee0::MSG_SCENE_NODE_CLEAR:
		ClearSceneNode();
		break;
	}
}

void WxStagePage::Traverse(std::function<bool(const ee0::GameObj&)> func,
	                       const ee0::VariantSet& variants,
	                       bool inverse) const
{
	auto var = variants.GetVariant("type");
	if (var.m_type == ee0::VT_EMPTY)
	{
		// todo ecs
#ifndef GAME_OBJ_ECS
		m_obj->GetSharedComp<n0::CompComplex>().Traverse(func, inverse);
#endif // GAME_OBJ_ECS
		return;
	}

	GD_ASSERT(var.m_type == ee0::VT_LONG, "err type");
	switch (var.m_val.l)
	{
	case TRAV_DRAW_PREVIEW:
		func(m_obj);
		break;
		// todo ecs
#ifndef GAME_OBJ_ECS
	default:
		m_obj->GetSharedComp<n0::CompComplex>().Traverse(func, inverse);
#endif // GAME_OBJ_ECS
	}
}

void WxStagePage::OnSetSkybox(const std::string& filepath)
{
    auto img_cube = facade::ResPool::Instance().Fetch<facade::ImageCube>(filepath);
    std::static_pointer_cast<ee3::WxStageCanvas>(GetImpl().GetCanvas())->SetSkybox(img_cube);
}

void WxStagePage::DoSimulationStep(float dt)
{
    // start
    for (auto& s : m_solvers) {
        s.second.StartSimulation(dt);
    }
    // wait
    for (auto& s : m_solvers) {
        s.second.WaitForSimulation();
    }
}

void WxStagePage::OnPageInit()
{
    InitClothObj();
}

#ifndef GAME_OBJ_ECS
const n0::NodeComp& WxStagePage::GetEditedObjComp() const
{
	return m_obj->GetSharedComp<n0::CompComplex>();
}
#endif // GAME_OBJ_ECS

void WxStagePage::StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
	                             rapidjson::MemoryPoolAllocator<>& alloc) const
{
	val.AddMember("page_type", rapidjson::Value(PAGE_TYPE.c_str(), alloc), alloc);
}

void WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	// todo ecs
#ifndef GAME_OBJ_ECS
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	if (m_selection.IsEmpty()) {
		ccomplex.AddChild(*obj);
	}
#endif // GAME_OBJ_ECS

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::DeleteSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	// todo ecs
#ifndef GAME_OBJ_ECS
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	if (ccomplex.RemoveChild(*obj)) {
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	}
#endif // GAME_OBJ_ECS
}

void WxStagePage::ClearSceneNode()
{
	// todo ecs
#ifndef GAME_OBJ_ECS
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	ccomplex.RemoveAllChildren();
	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
#endif // GAME_OBJ_ECS
}

void WxStagePage::InitClothObj()
{
    up::cloth::ClothMeshData cloth_mesh;

    auto translate = sm::mat4::Translated(-2.f, 13.f, 0.f);
    auto rotate = sm::mat4(sm::Quaternion::CreateFromAxisAngle(sm::vec3(1.f, 0.f, 0.f), SM_PI / 6.0f));
    auto transform = rotate * translate;

    cloth_mesh.GeneratePlaneCloth(6.f, 7.f, 49, 59, false, transform);
    cloth_mesh.AttachClothPlaneByAngles(49, 59);

    auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_MODEL);
    auto& ccloth = obj->AddUniqueComp<n3::CompCloth>();
    ccloth.BuildFromClothMesh(sm::vec3(0, 0, 0), *m_factory, cloth_mesh);

    auto solver = ccloth.GetSolver();
    m_solvers[solver].Initialize(solver.get(), &m_job_mgr);

    m_obj->GetSharedComp<n0::CompComplex>().AddChild(obj);
}

}
}