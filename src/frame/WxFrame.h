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
	virtual void OnOpen(wxCommandEvent& event) override;

	virtual void OnSettings(wxCommandEvent& event) override;

}; // WxFrame

}