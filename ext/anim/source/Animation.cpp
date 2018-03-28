#include "anim/Animation.h"
#include "anim/Layer.h"
#include "anim/KeyFrame.h"

#include <guard/check.h>
#include <node0/SceneNode.h>

namespace anim
{

Animation::Animation(const std::vector<LayerPtr>& layers, int max_frame_idx)
	: m_max_frame_idx(0)
	, m_max_item_num(max_frame_idx)
{
	SetCountNum(layers);
	FillingLayers(layers);
	ConnectItems(layers);
	LoadLerpData(layers);
	CreateSprSlots(layers);
}

void Animation::SetCountNum(const std::vector<LayerPtr>& layers)
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

void Animation::FillingLayers(const std::vector<LayerPtr>& layers)
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

void Animation::ConnectItems(const std::vector<LayerPtr>& layers)
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
			for (int icurr = 0, ncurr = curr.items.size(); icurr < ncurr; ++icurr) {
				for (int inext = 0, nnext = next.items.size(); inext < nnext; ++inext) {
					auto& curr_spr = src_frames[iframe]->GetAllNodes[icurr];
					auto& next_spr = src_frames[iframe+1]->GetAllNodes[inext];
					if (curr_spr->GetName() == next_spr->GetName()) {
						curr.items[icurr].next = inext;
						next.items[inext].prev = icurr;
						break;
					}
				}
			}
		}
	}
}

void Animation::LoadLerpData(const std::vector<LayerPtr>& layers)
{
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

				auto& begin = src_frames[iframe]->GetAllNodes[iitem];
				auto& end   = src_frames[iframe+1]->GetAllNodes[item.next];
				int dt = layer.frames[iframe + 1].frame_idx - layer.frames[iframe].frame_idx;

				SprSRT bsrt, esrt;
				begin->GetLocalSRT(bsrt);
				end->GetLocalSRT(esrt);
				dst.srt = bsrt;
				for (int i = 0; i < SprSRT::SRT_MAX; ++i) {
					dst.dsrt.srt[i] = (esrt.srt[i] - bsrt.srt[i]) / dt;
				}

				const RenderColor& rcb = begin->GetColor();
				const RenderColor& rce = end->GetColor();
				dst.col_mul = rcb.GetMul();
				dst.col_add = rcb.GetAdd();
				CalcDeltaColor(rcb.GetMul(), rce.GetMul(), dt, dst.dcol_mul);
				CalcDeltaColor(rcb.GetAdd(), rce.GetAdd(), dt, dst.dcol_add);

				m_lerps.push_back(dst);
				item.lerp = idx;
			}
		}
	}
}

void Animation::CreateSprSlots(const std::vector<LayerPtr>& layers)
{
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

/************************************************************************/
/* struct Animation::Item                                               */
/************************************************************************/

Animation::Item::Item()
	: slot(-1)
	, next(-1)
	, prev(-1)
	, lerp(-1)
{
}

}