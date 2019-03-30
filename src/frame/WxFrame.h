#pragma once

#include <ee0/WxFrame.h>

namespace eone
{

class WxFrame : public ee0::WxFrame
{
public:
	WxFrame();
    virtual ~WxFrame();

private:
    enum
    {
        ID_SKYBOX = 2000,
    };

protected:
	virtual void OnNew(wxCommandEvent& event) override;
	virtual void OnOpen(wxCommandEvent& event) override;

	virtual void OnSettings(wxCommandEvent& event) override;

private:
    wxMenu* InitToolsBar();

    void OnSetSkybox(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()

}; // WxFrame

}