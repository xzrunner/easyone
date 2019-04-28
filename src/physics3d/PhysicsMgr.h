#pragma once

#include <ee0/GameObj.h>

#include <SM_Vector.h>
#include <uniphysics/cloth/JobManager.h>

#include <boost/noncopyable.hpp>

#include <map>

namespace up
{
namespace cloth
{
    class Factory;
    class Cloth;
    class MultithreadedSolverHelper;
    class ClothMeshData;
}
namespace rigid
{
    class World;
    class Shape;
}
}

namespace eone
{
namespace physics3d
{

class PhysicsMgr : boost::noncopyable
{
public:
    PhysicsMgr();
    ~PhysicsMgr();

    ee0::GameObj CreateCloth(const sm::vec3& center,
        up::cloth::ClothMeshData& cloth_mesh);

    ee0::GameObj CreateBox(float mass, const sm::vec3& half_extents,
        const sm::vec3& pos, const sm::vec4& color);
    ee0::GameObj CreateBox(float mass, const up::rigid::Shape& shape,
        const sm::vec3& pos, const sm::vec4& color);

    void DoSimulationStep(float dt);

private:
    // rigid
    std::unique_ptr<up::rigid::World> m_world = nullptr;

    // cloth
    up::cloth::JobManager m_job_mgr;
    std::unique_ptr<up::cloth::Factory> m_factory = nullptr;
    std::map<std::shared_ptr<up::cloth::Solver>, up::cloth::MultithreadedSolverHelper> m_solvers;

}; // PhysicsMgr

}
}