#include "model/WxStageCanvas.h"
#include "model/skinned.glsl"
#include "model/shape.glsl"

#include "frame/WxStagePage.h"

#include <ee0/WxStagePage.h>

#include <model/Model.h>
#include <model/ModelExtendType.h>
#include <node0/SceneNode.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <unirender/Shader.h>
#include <painting2/PrimitiveDraw.h>
#include <painting3/PrimitiveDraw.h>
#include <painting3/EffectsManager.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>

namespace
{

std::shared_ptr<ur::Shader> DEFAULT_SHADER = nullptr;
std::shared_ptr<ur::Shader> SHAPE_SHADER   = nullptr;

}

namespace eone
{
namespace model
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
	                         const ee0::RenderContext& rc)
	: WxStageCanvas3D(stage, rc, false)
	, m_obj(stage->GetEditedObj())
{
}

void WxStageCanvas::DrawBackground() const
{
	if (!DEFAULT_SHADER) {
		InitShaders();
	}
}

void WxStageCanvas::DrawForeground() const
{
	n3::RenderParams params;
	params.mt   = m_camera->GetModelViewMat();
	params.type = n3::RenderParams::DRAW_MESH;

	if (!m_obj->HasSharedComp<n3::CompModel>()) {
		return;
	}

	auto& cmodel = m_obj->GetUniqueComp<n3::CompModelInst>();
	auto& model_inst = cmodel.GetModel();
	if (!model_inst) {
		return;
	}

	auto& model = model_inst->GetModel();
	auto& ext = model->ext;
	if (ext->Type() == ::model::EXT_SKELETAL) {
		DrawImpl(*model_inst, params);
	} else {
		n3::RenderSystem::Draw(m_obj, params);
	}
}

void WxStageCanvas::InitShaders() const
{
	std::vector<std::string> default_textures;

	CU_VEC<ur::VertexAttrib> default_layout;
	default_layout.push_back(ur::VertexAttrib("position",      3, 4, 40, 0));
	default_layout.push_back(ur::VertexAttrib("normal",        3, 4, 40, 12));
	default_layout.push_back(ur::VertexAttrib("texcoord",      2, 4, 40, 24));
	default_layout.push_back(ur::VertexAttrib("blend_indices", 4, 1, 40, 32));
	default_layout.push_back(ur::VertexAttrib("blend_weights", 4, 1, 40, 36));

	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	DEFAULT_SHADER = std::make_shared<ur::Shader>(
		&rc, model_skinned_vs, model_skinned_fs, default_textures, default_layout);
	SHAPE_SHADER = std::make_shared<ur::Shader>(
		&rc, model_shape_vs, model_shape_fs, default_textures, default_layout);
}

void WxStageCanvas::DrawImpl(const ::model::ModelInstance& model_inst,
                             const n3::RenderParams& params) const
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	auto mgr = pt3::EffectsManager::Instance();

	rc.SetPolygonMode(ur::POLYGON_LINE);
	pt2::PrimitiveDraw::LineWidth(1);
	mgr->SetUserEffect(std::static_pointer_cast<ur::Shader>(DEFAULT_SHADER));
	DrawSkeletalNode(model_inst, 0, params);
}

void WxStageCanvas::DrawSkeletalNode(const ::model::ModelInstance& model_inst,
	                                 int node_idx, const n3::RenderParams& params) const
{
	auto& model = *model_inst.GetModel();
	auto& g_trans = model_inst.GetGlobalTrans();
	auto& nodes = static_cast<::model::SkeletalAnim*>(model.ext.get())->GetAllNodes();
	auto& node = *nodes[node_idx];
	if (!node.children.empty())
	{
		assert(node.meshes.empty());
		for (auto& child : node.children) {
			DrawSkeletalNode(model_inst, child, params);
		}
	}
	else
	{
		auto mgr = pt3::EffectsManager::Instance();
		auto child_mat = g_trans[node_idx] * params.mt;
		assert(node.children.empty());
		for (auto& mesh_idx : node.meshes)
		{
			auto& mesh = model.meshes[mesh_idx];

			auto& material = model.materials[mesh->material];
			if (material->diffuse_tex != -1) {
				int tex_id = model.textures[material->diffuse_tex].second->TexID();
				ur::Blackboard::Instance()->GetRenderContext().BindTexture(tex_id, 0);
			}

			auto effect_type = pt3::EffectsManager::EffectType(mesh->effect);
			if (effect_type == pt3::EffectsManager::EFFECT_SKINNED) {
				effect_type = pt3::EffectsManager::EFFECT_USER;
			}
			auto effect = mgr->Use(effect_type);
			auto mode = effect->GetDrawMode();

			sm::mat4 world_mat;

			auto& bone_trans = model_inst.CalcBoneMatrices(node_idx, mesh_idx);
			if (!bone_trans.empty()) {
				mgr->SetBoneMatrixes(effect_type, &bone_trans[0], bone_trans.size());
			} else {
				sm::mat4 mat;
				mgr->SetBoneMatrixes(effect_type, &mat, 1);
			}

			mgr->SetLightPosition(effect_type, sm::vec3(0.25f, 0.25f, 1));
			mgr->SetProjMat(effect_type, pt3::Blackboard::Instance()->GetWindowContext()->GetProjMat().x);
			mgr->SetNormalMat(effect_type, child_mat);

			mgr->SetMaterial(effect_type, material->ambient, material->diffuse,
				material->specular, material->shininess);

			mgr->SetModelViewMat(effect_type, child_mat.x);

			auto& geo = mesh->geometry;
			for (auto& sub : geo.sub_geometries)
			{
				if (sub.index) {
					ur::Blackboard::Instance()->GetRenderContext().DrawElementsVAO(
						mode, sub.offset, sub.count, geo.vao);
				} else {
					ur::Blackboard::Instance()->GetRenderContext().DrawArraysVAO(
						mode, sub.offset, sub.count, geo.vao);
				}
			}
		}
	}
}

}
}