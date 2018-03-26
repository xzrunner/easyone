#include "frame/main.h"
#include "frame/WxFrame.h"
#include "frame/Application.h"
#include "frame/Blackboard.h"
#include "frame/WxStagePage.h"
#include "frame/WxStagePanel.h"

#include <ee0/WxFrame.h>
#include <ee0/SubjectMgr.h>

#ifdef _DEBUG
IMPLEMENT_APP_CONSOLE(MyApp);
#else
IMPLEMENT_APP(MyApp);
#endif

bool MyApp::OnInit()
{
	auto bb = eone::Blackboard::Instance();
	auto frame = new eone::WxFrame();
	bb->SetFrame(frame);
	auto app = std::make_shared<eone::Application>(frame);
	bb->SetApp(app);
	frame->SetApp(app);
	frame->Show(true);

	auto stage = app->GetStagePanel();
	auto curr_page = static_cast<eone::WxStagePage*>(stage->GetPage(stage->GetSelection()));
	curr_page->GetSubjectMgr()->NotifyObservers(ee0::MSG_STAGE_PAGE_ON_SHOW);

	return true;
}