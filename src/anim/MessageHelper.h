#pragma once

namespace ee0 { class SubjectMgr; }

namespace eone
{
namespace anim
{

class MessageHelper
{
public:
	static void SetSelectedRegion(ee0::SubjectMgr& sub_mgr, int col);
	static void SetCurrFrame(ee0::SubjectMgr& sub_mgr, int layer, int frame);

}; // MessageHelper

}
}