#include "frame/WxLibraryPanel.h"

#include <ee0/WxLibraryPage.h>
#include <ee3/WxLibGeoPage.h>

namespace eone
{

WxLibraryPanel::WxLibraryPanel(wxWindow* parent)
	: ee0::WxLibraryPanel(parent)
{
	wxWindow* nb = GetNotebook();
	AddPage(new ee0::WxLibraryPage(nb, "Any"));
	AddPage(new ee3::WxLibGeoPage(nb));
}

}