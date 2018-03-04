#pragma once

namespace eone
{

class WxStagePanel;
class WxStagePage;

class StagePageFactory
{
public:
	static WxStagePage* Create(int page_type, WxStagePanel* stage_panel);

	static void CreatePreviewOP();

}; // StagePageFactory

}