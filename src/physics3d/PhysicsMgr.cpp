#include "physics3d/PhysicsMgr.h"

#include "frame/GameObjFactory.h"

#include <uniphysics/rigid/bullet/World.h>
#include <uniphysics/rigid/bullet/Shape.h>
#include <uniphysics/rigid/bullet/Body.h>
#include <uniphysics/cloth/ClothMeshData.h>
#include <uniphysics/cloth/Solver.h>
#include <uniphysics/cloth/nv/Factory.h>
#include <model/MeshBuider.h>
#include <node0/SceneNode.h>
#include <node3/CompCloth.h>
#include <node3/CompRigid.h>
#include <node3/CompModel.h>

#include <NvCloth/Factory.h>

namespace eone
{
namespace physics3d
{

PhysicsMgr::PhysicsMgr()
{
    m_world = std::make_unique<up::rigid::bullet::World>();
    m_factory = std::make_unique<up::cloth::nv::Factory>(nv::cloth::Platform::CPU);
}

PhysicsMgr::~PhysicsMgr()
{
}

ee0::GameObj PhysicsMgr::CreateCloth(const sm::vec3& center, up::cloth::ClothMeshData& cloth_mesh)
{
    auto obj = eone::GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_MODEL);
    auto& ccloth = obj->AddUniqueComp<n3::CompCloth>();
    ccloth.BuildFromClothMesh(sm::vec3(0, 0, 0), *m_factory, cloth_mesh);

    auto solver = ccloth.GetSolver();
    m_solvers[solver].Initialize(solver.get(), &m_job_mgr);

    return obj;
}

ee0::GameObj PhysicsMgr::CreateBox(float mass, const sm::vec3& half_extents,
                                   const sm::vec3& pos, const sm::vec4& color)
{
    auto shape = std::make_shared<up::rigid::bullet::Shape>();
    shape->InitBoxShape(half_extents);
    auto body = std::make_shared<up::rigid::bullet::Body>(mass, pos, shape, color);
    m_world->AddBody(body);

    auto obj = eone::GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_MODEL);
    auto& crigid = obj->AddUniqueComp<n3::CompRigid>();
    crigid.SetBody(body);

    return obj;
}

ee0::GameObj PhysicsMgr::CreateBox(float mass, const std::shared_ptr<up::rigid::Shape>& shape,
                                   const sm::vec3& pos, const sm::vec4& color,
                                   const n0::CompAssetPtr& model_comp)
{
    auto body = std::make_shared<up::rigid::bullet::Body>(mass, pos, shape, color);
    m_world->AddBody(body);

    auto obj = eone::GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_MODEL, model_comp);
    auto& crigid = obj->AddUniqueComp<n3::CompRigid>();
    crigid.SetBody(body);

    return obj;
}

void PhysicsMgr::DoSimulationStep(float dt)
{
    m_world->StepSimulation(dt);

    // start
    for (auto& s : m_solvers) {
        s.second.StartSimulation(dt);
    }
    // wait
    for (auto& s : m_solvers) {
        s.second.WaitForSimulation();
    }
}

}
}