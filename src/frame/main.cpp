#include "frame/main.h"
#include "frame/WxFrame.h"
#include "frame/Application.h"
#include "frame/Blackboard.h"

#include <ee0/WxFrame.h>

#ifdef _DEBUG
IMPLEMENT_APP_CONSOLE(MyApp);
#else
IMPLEMENT_APP(MyApp);
#endif

bool MyApp::OnInit()
{
	auto frame = new eone::WxFrame();
	eone::Blackboard::Instance()->SetFrame(frame);
	auto app = std::make_shared<eone::Application>(frame);
	frame->SetApp(app);
	frame->Show(true);

	return true;
}