#include "pch_script.h"
#include "UIGameCustom.h"
#include "level.h"
#include "hudmanager.h"
#include "ui/uistatic.h"

using namespace luabind;

CUIGameCustom* get_hud(){
	return CurrentGameUI();
}

#pragma optimize("s",on)
void CUIGameCustom::script_register(lua_State *L)
{
	module(L)
		[
			class_< SDrawStaticStruct >("SDrawStaticStruct")
			.def_readwrite("m_endTime",		&SDrawStaticStruct::m_endTime)
			.def("wnd",					&SDrawStaticStruct::wnd),

			class_< CUIGameCustom >("CUIGameCustom")
			.def("AddDialogToRender",		&CUIGameCustom::AddDialogToRender)
			.def("RemoveDialogToRender",	&CUIGameCustom::RemoveDialogToRender)
			.def("AddCustomStatic",			&CUIGameCustom::AddCustomStatic)
			.def("RemoveCustomStatic",		&CUIGameCustom::RemoveCustomStatic)
			.def("GetCustomStatic",			&CUIGameCustom::GetCustomStatic),
			def("get_hud",					&get_hud)
		];
}
