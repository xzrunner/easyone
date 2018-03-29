#include "anim/AnimTemplate.h"
#include "anim/Layer.h"
#include "anim/KeyFrame.h"
#include "anim/Utility.h"

// todo
#include <ee0/CompNodeEditor.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node2/CompColorCommon.h>
#include <node2/CompColorMap.h>
#include <node2/CompTransform.h>

namespace anim
{

AnimTemplate::AnimTemplate()
	: m_max_item_num(0)
	, m_max_frame_idx(0)
{
}

void AnimTemplate::Clear()
{
	m_layers.clear();
	m_slots.clear();
	m_lerps.clear();
	m_max_frame_idx = 0;
	m_max_item_num = 0;
}

void AnimTemplate::Build(const std::vector<LayerPtr>& layers)
{
	m_max_item_num = 0;
	m_max_frame_idx = Utility::GetMaxFrame(layers);

	SetCountNum(layers);
	FillingLayers(layers);
	ConnectItems(layers);
	LoadLerpData(layers);
	CreateSprSlots(layers);
}

void AnimTemplate::SetCountNum(const std::vector<LayerPtr>& layers)
{
	for (auto& layer : layers) 
	{
		int max_count = -1;
		for (auto& frame : layer->GetAllKeyFrames()) 
		{
			int count = frame->GetAllNodes().size();
			if (count > max_count) {
				max_count = count;
			}
		}
		m_max_item_num += max_count;
	}
}

void AnimTemplate::FillingLayers(const std::vector<LayerPtr>& layers)
{
	m_layers.clear();
	m_layers.resize(layers.size());
	for (int ilayer = 0, nlayer = layers.size(); ilayer < nlayer; ++ilayer) 
	{
		auto& src_layer = layers[ilayer];
		Layer& dst_layer = m_layers[ilayer];
		size_t frame_sz = src_layer->GetAllKeyFrames().size();
		dst_layer.frames.resize(frame_sz);
		for (int iframe = 0, nframe = frame_sz; iframe < nframe; ++iframe)
		{
			const auto& src_frame = src_layer->GetAllKeyFrames()[iframe];
			Frame& dst_frame = dst_layer.frames[iframe];
			dst_frame.frame_idx = src_frame->GetFrameIdx();

			auto& nodes = src_frame->GetAllNodes();
			size_t node_sz = nodes.size();
			dst_frame.items.resize(node_sz);
			for (int iitem = 0, nitem = node_sz; iitem < nitem; ++iitem) {
				dst_frame.items[iitem].node = nodes[iitem];
			}

			//dst_frame.lerps.reserve(src_frame->lerps.size());
			//for (int ilerp = 0, nlerp = src_frame->lerps.size(); ilerp < nlerp; ++ilerp) {
			//	AnimLerp::SprData type = src_frame->lerps[ilerp].first;
			//	std::unique_ptr<ILerp> lerp = src_frame->lerps[ilerp].second->Clone();
			//	dst_frame.lerps.push_back(std::make_pair(type, std::move(lerp)));
			//}
		}
	}
}

void AnimTemplate::ConnectItems(const std::vector<LayerPtr>& layers)
{
	for (int ilayer = 0, nlayer = m_layers.size(); ilayer < nlayer; ++ilayer) 
	{
		Layer& layer = m_layers[ilayer];
		if (layer.frames.size() <= 1) {
			continue;
		}
		auto& src_layer = layers[ilayer];
		auto& src_frames = src_layer->GetAllKeyFrames();
		for (int iframe = 0, nframe = layer.frames.size(); iframe < nframe - 1; ++iframe) 
		{
			if (!src_frames[iframe]->GetTween()) {
				continue;
			}

			Frame& curr = layer.frames[iframe];
			Frame& next = layer.frames[iframe + 1];
			for (int icurr = 0, ncurr = curr.items.size(); icurr < ncurr; ++icurr) 
			{
				for (int inext = 0, nnext = next.items.size(); inext < nnext; ++inext) 
				{
					auto& curr_node = src_frames[iframe]->GetAllNodes()[icurr];
					auto& next_node = src_frames[iframe+1]->GetAllNodes()[inext];
					auto& curr_info = curr_node->GetUniqueComp<ee0::CompNodeEditor>();
					auto& next_info = next_node->GetUniqueComp<ee0::CompNodeEditor>();
					if (curr_info.GetName() == next_info.GetName())
					{
						curr.items[icurr].next = inext;
						next.items[inext].prev = icurr;
						break;
					}
				}
			}
		}
	}
}

void AnimTemplate::LoadLerpData(const std::vector<LayerPtr>& layers)
{
	m_lerps.clear();
	for (int ilayer = 0, nlayer = m_layers.size(); ilayer < nlayer; ++ilayer) 
	{
		auto& src_frames = layers[ilayer]->GetAllKeyFrames();
		Layer& layer = m_layers[ilayer];
		for (int iframe = 0, nframe = layer.frames.size(); iframe < nframe; ++iframe) 
		{
			Frame& frame = layer.frames[iframe];
			for (int iitem = 0, nitem = frame.items.size(); iitem < nitem; ++iitem) 
			{
				Item& item = frame.items[iitem];
				if (item.next == -1) {
					continue;
				}
				GD_ASSERT(item.lerp == -1, "err item");

				int idx = m_lerps.size();

				Lerp dst;

				auto& begin = src_frames[iframe]->GetAllNodes()[iitem];
				auto& end   = src_frames[iframe+1]->GetAllNodes()[item.next];

				auto& b_ctrans = begin->GetUniqueComp<n2::CompTransform>();
				auto& e_ctrans = end->GetUniqueComp<n2::CompTransform>();
				auto& b_srt = b_ctrans.GetTrans().GetSRT();
				auto& e_srt = e_ctrans.GetTrans().GetSRT();
				int dt = layer.frames[iframe + 1].frame_idx - layer.frames[iframe].frame_idx;
				dst.srt = b_srt;
				dst.dsrt = (e_srt - b_srt) / static_cast<float>(dt);

				if (begin->HasUniqueComp<n2::CompColorCommon>())
				{
					auto& b_ccol = begin->GetUniqueComp<n2::CompColorCommon>();
					auto& e_ccol = end->GetUniqueComp<n2::CompColorCommon>();
					dst.col_mul = b_ccol.GetColor().mul;
					dst.col_add = b_ccol.GetColor().add;
					CalcDeltaColor(b_ccol.GetColor().mul, e_ccol.GetColor().mul, dt, dst.dcol_mul);
					CalcDeltaColor(b_ccol.GetColor().add, e_ccol.GetColor().add, dt, dst.dcol_add);
				}

				m_lerps.push_back(dst);
				item.lerp = idx;
			}
		}
	}
}

void AnimTemplate::CreateSprSlots(const std::vector<LayerPtr>& layers)
{
	m_slots.clear();
	for (int ilayer = 0, nlayer = m_layers.size(); ilayer < nlayer; ++ilayer) 
	{
		auto& src_frames = layers[ilayer]->GetAllKeyFrames();
		Layer& layer = m_layers[ilayer];
		for (int iframe = 0, nframe = layer.frames.size(); iframe < nframe; ++iframe) 
		{
			auto& src_nodes = src_frames[iframe]->GetAllNodes();
			Frame& frame = layer.frames[iframe];
			for (int iitem = 0, nitem = frame.items.size(); iitem < nitem; ++iitem) 
			{
				Item& item = frame.items[iitem];
				if (item.slot != -1) {
					continue;
				}
				int slot = m_slots.size();
				
				m_slots.push_back(src_nodes[iitem]->Clone());
				item.slot = slot;

				Item* ptr = &item;
				int idx = iframe;
				while (ptr->next != -1 && idx < nframe - 1) 
				{
					ptr = &layer.frames[++idx].items[ptr->next];
					ptr->slot = slot;
				}
			}
		}
	}
}

void AnimTemplate::CalcDeltaColor(const pt2::Color& begin, const pt2::Color& end, int time, float* ret)
{
	ret[0] = (end.r - begin.r) / (float)time;
	ret[1] = (end.g - begin.g) / (float)time;
	ret[2] = (end.b - begin.b) / (float)time;
	ret[3] = (end.a - begin.a) / (float)time;
}

/************************************************************************/
/* struct AnimTemplate::Item                                               */
/************************************************************************/

AnimTemplate::Item::Item()
	: slot(-1)
	, next(-1)
	, prev(-1)
	, lerp(-1)
{
}

}