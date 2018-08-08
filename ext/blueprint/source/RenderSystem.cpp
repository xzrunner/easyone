//#include "blueprint/RenderSystem.h"
//#include "blueprint/CompNode.h"
//#include "blueprint/CompGraph.h"
//
//#include <guard/check.h>
//#include <node0/SceneNode.h>
//#include <node0/CompAsset.h>
//#include <node2/CompTransform.h>
//#include <painting2/PrimitiveDraw.h>
//
//namespace bp
//{
//
//void RenderSystem::Draw(const n0::SceneNodePtr& node,
//	                    const sm::Matrix2D& mat)
//{
//	// matrix
//	sm::Matrix2D cmat = mat;
//	if (node->HasUniqueComp<n2::CompTransform>())
//	{
//		auto& ctrans = node->GetUniqueComp<n2::CompTransform>();
//		cmat = ctrans.GetTrans().GetMatrix() * cmat;
//	}
//
//	// asset
//	auto& casset = node->GetSharedComp<n0::CompAsset>();
//	auto asset_type = casset.AssetTypeID();
//	if (asset_type == n0::GetAssetUniqueTypeID<bp::CompGraph>())
//	{
//		auto& cgraph = static_cast<const bp::CompGraph&>(casset);
//		DrawGraph(cgraph.GetGraph(), cmat);
//	}
//	else if (asset_type == n0::GetAssetUniqueTypeID<bp::CompNode>())
//	{
//		auto& cnode = static_cast<const bp::CompNode&>(casset);
//		DrawNode(cnode.GetNode(), cmat);
//	}
//	else
//	{
//		GD_REPORT_ASSERT("err asset type.");
//	}
//}
//
//void RenderSystem::DrawGraph(const Graph& graph, const sm::Matrix2D& mat)
//{
//}
//
//void RenderSystem::DrawNode(const Node& node, const sm::Matrix2D& mat)
//{
//	auto& style = node.GetStyle();
//
//	float hw = style.size.x * 0.5f;
//	float hh = style.size.y * 0.5f;
//	sm::rect r(mat * sm::vec2(-hw, -hh), mat * sm::vec2(hw, hh));
//
//	pt2::PrimitiveDraw::SetColor(style.color);
//	pt2::PrimitiveDraw::Rect(nullptr, r, true);
//}
//
//}