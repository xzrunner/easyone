#include "frame/GameObjFactory.h"
#include "frame/config.h"
#include "frame/Blackboard.h"

#ifdef MODULE_PARTICLE3D
#include "particle3d/config.h"
#endif // MODULE_PARTICLE3D

#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node0/CompIdentity.h>
#include <node0/CompMaterial.h>
#include <node2/CompImage.h>
#include <node2/CompText.h>
#include <node2/CompMask.h>
#include <node2/CompMesh.h>
#include <node2/CompScale9.h>
#include <node2/CompAnim.h>
#include <node2/CompAnimInst.h>
#include <node2/CompParticle3d.h>
#include <node2/CompParticle3dInst.h>
#include <node2/CompBoundingBox.h>
#include <node2/CompTransform.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <node3/CompTransform.h>
#include <node3/CompAABB.h>
#include <node3/CompImage3D.h>
#include <node3/CompLight.h>
#include <node3/CompMeshFilter.h>
#else
#include <entity0/World.h>
#include <entity2/CompImage.h>
#include <entity2/CompText.h>
#include <entity2/CompMask.h>
#include <entity2/CompMesh.h>
#include <entity2/CompScale9.h>
#include <entity2/CompComplex.h>
#include <entity2/CompBoundingBox.h>
#include <entity2/CompTransform.h>
#endif // GAME_OBJ_ECS

#ifdef MODULE_ANIM
#include <anim/KeyFrame.h>
#include <anim/Layer.h>
#endif // MODULE_ANIM
#include <emitter/P3dTemplate.h>
#include <painting3/PointLight.h>
#include <painting3/MaterialMgr.h>
#include <model/ParametricEquations.h>

namespace eone
{

ee0::GameObj GameObjFactory::Create(ECS_WORLD_PARAM GameObjType type,
                                    const std::shared_ptr<n0::CompAsset>& shared_comp)
{
	if (type == GAME_OBJ_UNKNOWN) {
		return GAME_OBJ_NULL;
	}

#ifndef GAME_OBJ_ECS
	ee0::GameObj obj = std::make_shared<n0::SceneNode>();
#else
	auto obj = world.CreateEntity();
#endif // GAME_OBJ_ECS

	bool is_2d = true;

	sm::rect sz;
	switch (type)
	{
	case GAME_OBJ_IMAGE:
		{
#ifndef GAME_OBJ_ECS
            if (shared_comp) {
                obj->AddSharedCompNoCreate(shared_comp);
            } else {
                obj->AddSharedComp<n2::CompImage>();
            }
#else
			world.AddComponent<e2::CompImage>(obj);
#endif // GAME_OBJ_ECS
			sz.Build(100, 100);
		}
		break;
	case GAME_OBJ_IMAGE3D:
        if (shared_comp) {
            obj->AddSharedCompNoCreate(shared_comp);
        } else {
            obj->AddSharedComp<n3::CompImage3D>();
        }
		break;
	case GAME_OBJ_TEXT:
		{
#ifndef GAME_OBJ_ECS
            if (shared_comp) {
                obj->AddSharedCompNoCreate(shared_comp);
            } else {
                obj->AddSharedComp<n2::CompText>();
            }
            auto& tb = obj->GetSharedComp<n2::CompText>().GetText().tb;
#else
			auto& ctext = world.AddComponent<e2::CompText>(obj);
			auto& tb = ctext.text.tb;
#endif // GAME_OBJ_ECS
			sz.Build(static_cast<float>(tb.width), static_cast<float>(tb.height));
		}
		break;
	case GAME_OBJ_MASK:
		{
#ifndef GAME_OBJ_ECS
            if (shared_comp) {
                obj->AddSharedCompNoCreate(shared_comp);
            } else {
                obj->AddSharedComp<n2::CompMask>();
            }
#else
			world.AddComponent<e2::CompMask>(obj);
#endif // GAME_OBJ_ECS
			sz.Build(100, 100);
		}
		break;
	case GAME_OBJ_MESH:
		{
#ifndef GAME_OBJ_ECS
            if (shared_comp) {
                obj->AddSharedCompNoCreate(shared_comp);
            } else {
                obj->AddSharedComp<n2::CompMesh>();
            }
#else
			world.AddComponent<e2::CompMesh>(obj);
#endif // GAME_OBJ_ECS
			sz.Build(100, 100);
		}
		break;
	case GAME_OBJ_SCALE9:
		{
#ifndef GAME_OBJ_ECS
            if (shared_comp) {
                obj->AddSharedCompNoCreate(shared_comp);
            } else {
                obj->AddSharedComp<n2::CompScale9>();
            }
#else
			world.AddComponent<e2::CompScale9>(obj);
#endif // GAME_OBJ_ECS
			sz.Build(100, 100);
		}
		break;
#ifdef MODULE_ANIM
	case GAME_OBJ_ANIM:
		{
#ifndef GAME_OBJ_ECS
			sz.Build(100, 100);

			auto layer = std::make_unique<anim::Layer>();
			layer->SetName("Layer0");

			layer->AddKeyFrame(std::make_unique<anim::KeyFrame>(0));

            if (shared_comp) {
                obj->AddSharedCompNoCreate(shared_comp);
            } else {
                obj->AddSharedComp<n2::CompAnim>();
            }
			auto& canim = obj->GetSharedComp<n2::CompAnim>();
			canim.AddLayer(layer);

			obj->AddUniqueComp<n2::CompAnimInst>(canim.GetAnimTemplate());
#endif // GAME_OBJ_ECS
		}
		break;
#endif // MODULE_ANIM
#ifdef MODULE_PARTICLE3D
	case GAME_OBJ_PARTICLE3D:
		{
#ifndef GAME_OBJ_ECS
			sz.Build(100, 100);

            if (shared_comp) {
                obj->AddSharedCompNoCreate(shared_comp);
            } else {
                obj->AddSharedComp<n2::CompParticle3d>(particle3d::MAX_COMPONENTS);
            }
			auto& cp3d = obj->GetSharedCompPtr<n2::CompParticle3d>();
			obj->AddUniqueComp<n2::CompParticle3dInst>(cp3d);
#endif // GAME_OBJ_ECS
		}
		break;
#endif // MODULE_PARTICLE3D

#ifdef MODULE_MODEL
	case GAME_OBJ_MODEL:
		{
			is_2d = false;
#ifndef GAME_OBJ_ECS
            if (shared_comp) {
                obj->AddSharedCompNoCreate(shared_comp);
                auto& cmodel_inst = obj->AddUniqueComp<n3::CompModelInst>();
                auto model = std::static_pointer_cast<n3::CompModel>(shared_comp)->GetModel();
                if (model) {
                    cmodel_inst.SetModel(model, 0);
                }
            } else {
                obj->AddSharedComp<n3::CompModel>();
                obj->AddUniqueComp<n3::CompModelInst>();
            }
#endif // GAME_OBJ_ECS
		}
		break;
#endif // MODULE_MODEL

    case GAME_OBJ_LIGHT:
    {
        is_2d = false;

        auto& clight = obj->AddUniqueComp<n3::CompLight>();
        auto light = std::make_shared<pt3::PointLight>(sm::vec3(0, 2, -4));
        clight.SetLight(light);

        auto& cid = obj->AddUniqueComp<n0::CompIdentity>();
        cid.SetName("light");

        auto& cmesh = obj->AddUniqueComp<n3::CompMeshFilter>();
        auto dev = Blackboard::Instance()->GetRenderDevice();
        cmesh.SetMesh(*dev, model::Sphere::TYPE_NAME);

        auto& caabb = obj->AddUniqueComp<n3::CompAABB>();
        caabb.SetAABB(cmesh.GetAABB());

        auto& cmaterial = obj->AddUniqueComp<n0::CompMaterial>();
        auto mat = std::make_unique<pt0::Material>();
        typedef pt3::MaterialMgr::PhongUniforms UNIFORMS;
        mat->AddVar(UNIFORMS::ambient.name,     pt0::RenderVariant(sm::vec3(0.04f, 0.04f, 0.04f)));
        mat->AddVar(UNIFORMS::diffuse.name,     pt0::RenderVariant(sm::vec3(1, 1, 1)));
        mat->AddVar(UNIFORMS::specular.name,    pt0::RenderVariant(sm::vec3(1, 1, 1)));
        mat->AddVar(UNIFORMS::shininess.name,   pt0::RenderVariant(50.0f));
        cmaterial.SetMaterial(mat);

        const float scale = 0.1f;
        auto& ctrans = obj->AddUniqueComp<n3::CompTransform>();
        ctrans.SetScale(sm::vec3(scale, scale, scale));
    }
        break;

    case GAME_OBJ_SPHERE:
    {
        is_2d = false;

        auto& cid = obj->AddUniqueComp<n0::CompIdentity>();
        cid.SetFilepath(model::Sphere::TYPE_NAME);
        cid.SetName(model::Sphere::TYPE_NAME);

        auto& cmesh = obj->AddUniqueComp<n3::CompMeshFilter>();
        auto dev = Blackboard::Instance()->GetRenderDevice();
        cmesh.SetMesh(*dev, model::Sphere::TYPE_NAME);

        auto& caabb = obj->AddUniqueComp<n3::CompAABB>();
        caabb.SetAABB(cmesh.GetAABB());

        auto& cmaterial = obj->AddUniqueComp<n0::CompMaterial>();
        auto mat = std::make_unique<pt0::Material>();
        typedef pt3::MaterialMgr::PhongUniforms UNIFORMS;
        mat->AddVar(UNIFORMS::ambient.name,     pt0::RenderVariant(sm::vec3(0.04f, 0.04f, 0.04f)));
        mat->AddVar(UNIFORMS::diffuse.name,     pt0::RenderVariant(sm::vec3(1, 1, 1)));
        mat->AddVar(UNIFORMS::specular.name,    pt0::RenderVariant(sm::vec3(1, 1, 1)));
        mat->AddVar(UNIFORMS::shininess.name,   pt0::RenderVariant(50.0f));
        cmaterial.SetMaterial(mat);
    }
        break;

	case GAME_OBJ_COMPLEX:
		GD_REPORT_ASSERT("err type.");
		break;
	case GAME_OBJ_COMPLEX2D:
		is_2d = true;
#ifndef GAME_OBJ_ECS
        if (shared_comp) {
            obj->AddSharedCompNoCreate(shared_comp);
        } else {
            obj->AddSharedComp<n0::CompComplex>();
        }
#else
		world.AddComponent<e2::CompComplex>(obj);
#endif // GAME_OBJ_ECS
		sz.Build(100, 100);
		break;
	case GAME_OBJ_COMPLEX3D:
		is_2d = false;
#ifndef GAME_OBJ_ECS
        if (shared_comp) {
            obj->AddSharedCompNoCreate(shared_comp);
        } else {
            obj->AddSharedComp<n0::CompComplex>();
        }
#else
		world.AddComponent<e2::CompComplex>(obj);
#endif // GAME_OBJ_ECS
		break;
	}

	// transform
	if (is_2d)
	{
#ifndef GAME_OBJ_ECS
        if (!obj->HasUniqueComp<n2::CompTransform>()) {
            obj->AddUniqueComp<n2::CompTransform>();
        }
#else
		world.AddComponent<e2::CompLocalMat>(obj);
#endif // GAME_OBJ_ECS
	}
	else
	{
#ifndef GAME_OBJ_ECS
        if (!obj->HasUniqueComp<n3::CompTransform>()) {
            obj->AddUniqueComp<n3::CompTransform>();
        }
#else
		world.AddComponent<e3::CompLocalMat>(obj);
#endif // GAME_OBJ_ECS
	}

	// aabb
	if (is_2d)
	{
#ifndef GAME_OBJ_ECS
        if (obj->HasUniqueComp<n2::CompBoundingBox>()) {
            obj->AddUniqueComp<n2::CompBoundingBox>(sz);
        }
#else
		world.AddComponent<e2::CompBoundingBox>(obj, sz);
#endif // GAME_OBJ_ECS
	}
	else
	{
#ifndef GAME_OBJ_ECS
        if (!obj->HasUniqueComp<n3::CompAABB>()) {
            obj->AddUniqueComp<n3::CompAABB>(sm::cube(1, 1, 1));
        }
#else
		world.AddComponent<e3::CompBoundingBox>(obj, sz);
#endif // GAME_OBJ_ECS
	}

	// id
#ifndef GAME_OBJ_ECS
    if (!obj->HasUniqueComp<n0::CompIdentity>()) {
        obj->AddUniqueComp<n0::CompIdentity>();
    }
#else
#endif // GAME_OBJ_ECS

	return obj;
}

}