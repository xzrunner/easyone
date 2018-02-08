#include "main.h"
#include "Task.h"

#include <ee0/WxFrame.h>

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	auto frame = new ee0::WxFrame("EasyOne", true);
	auto task = new eone::Task(frame);
	// todo zz
//	frame->SetTask(task);
	frame->Show(true);

	return true;
}