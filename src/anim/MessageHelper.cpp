#include "anim/MessageHelper.h"
#include "anim/MessageID.h"

#include <ee0/VariantSet.h>
#include <ee0/SubjectMgr.h>

namespace eone
{
namespace anim
{

void MessageHelper::SetSelectedRegion(ee0::SubjectMgr& sub_mgr, int col)
{
	ee0::VariantSet vars;

	ee0::Variant var;
	var.m_type = ee0::VT_INT;
	var.m_val.l = col;
	vars.SetVariant("col", var);

	sub_mgr.NotifyObservers(MSG_SET_SELECTED_REGION, vars);
}

void MessageHelper::SetCurrFrame(ee0::SubjectMgr& sub_mgr,
	                             int layer, int frame)
{
	ee0::VariantSet vars;

	ee0::Variant var_layer;
	var_layer.m_type = ee0::VT_INT;
	var_layer.m_val.l = layer;
	vars.SetVariant("layer", var_layer);

	ee0::Variant var_frame;
	var_frame.m_type = ee0::VT_INT;
	var_frame.m_val.l = frame;
	vars.SetVariant("frame", var_frame);

	sub_mgr.NotifyObservers(MSG_SET_CURR_FRAME, vars);
}

}
}