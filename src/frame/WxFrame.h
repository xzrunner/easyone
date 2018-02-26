#pragma once

#include <ee0/WxFrame.h>

namespace eone
{

class WxFrame : public ee0::WxFrame
{
public:
	WxFrame();

protected:
	virtual void OnNew(wxCommandEvent& event) override;

}; // WxFrame

}