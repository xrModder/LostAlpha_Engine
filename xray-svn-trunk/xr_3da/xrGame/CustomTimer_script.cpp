#include "stdafx.h"
#include "CustomTimer.h"
#include "pch_script.h"

using namespace luabind;

#pragma optimize("s",on)
void CTimerCustom::script_register(lua_State *L)
{
	module(L)
		[

			class_<CTimerCustom>("Timer")
				.def(constructor<>())
				.def("SetName",						&CTimerCustom::SetName)
				.def("Name",						&CTimerCustom::Name)
				.def("SetRealTime",					&CTimerCustom::SetRealTime)
				.def("SetGameTime",					&CTimerCustom::SetGameTime)
				.def("Time",						&CTimerCustom::Time)
				.def("TimeElapsed",					&CTimerCustom::TimeElapsed)
				.def("SetAction",					(void (CTimerCustom::*)(LPCSTR)) &CTimerCustom::SetAction)
				.def("Action",						&CTimerCustom::ActionScript)
				.def("SetInfo",						&CTimerCustom::SetInfo)
				.def("Info",						&CTimerCustom::InfoScript)
				.def("SetArgs",						&CTimerCustom::SetArgs)
				.def("GetArgs",						&CTimerCustom::ArgsScript)

				
				.def_readwrite("day",				&CTimerCustom::m_day)
				.def_readwrite("hour",				&CTimerCustom::m_hour)
				.def_readwrite("min",				&CTimerCustom::m_min)
				.def_readwrite("sec",				&CTimerCustom::m_sec)
				.def_readwrite("ms",				&CTimerCustom::m_ms)
				.def_readwrite("game_time",				&CTimerCustom::m_game_time)

				.def("SetHUD",						&CTimerCustom::SetHUD)
				.def("isHUD",						&CTimerCustom::isHUD)
				//.def("SetAutoRestart",				&CTimerCustom::SetAutoRestart)
				//.def("AutoRestart",					&CTimerCustom::AutoRestart)
				.def("Valide",						&CTimerCustom::valide)
				.def(self == other<CTimerCustom>())
		];
}


