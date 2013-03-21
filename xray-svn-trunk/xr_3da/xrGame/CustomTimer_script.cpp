#include "stdafx.h"
#include "CustomTimer.h"
#include "pch_script.h"
#include "actor.h"

using namespace luabind;


void CTimerCustom::StartAction()
{
	if (Action().size()) {
		luabind::functor<void>			fl;
		R_ASSERT3							(ai().script_engine().functor<void>(ActionScript(),fl),"Can't find function ",ActionScript());
		if (m_argument.type()!=LUA_TNIL)
			fl	(GetArgument());
		else
			fl	();
	}
	if (Info().size()) {
		Actor()->TransferInfo(InfoScript(),true);
	}
}

#pragma todo("gr1ph: optimise numbers recognition")

void CTimerCustom::store_table(IWriter &w)
{
	switch (m_argument.type())
	{
		case LUA_TTABLE: 
		{
			m_argument.pushvalue();
			int size = luaL_getn(m_argument.lua_state(), 1);
			w.w_u16(size);
			if (!size)
				return;
			for (luabind::object::iterator it = m_argument.begin(), last = m_argument.end();
					it != last;
					++it)
			{
				LPCSTR				key		= luabind::object_cast<LPCSTR>		(it.key());
				luabind::object	val			= (*it);
				u8					type	= (u8) val.type						();
				w.w_stringZ														(key);
				w.w_u8															(type);
				switch (type)
				{
					case LUA_TBOOLEAN: 
						w.w_bool												(luabind::object_cast<bool>(val));	
						break;
					case LUA_TNUMBER:
						w.w_double												(luabind::object_cast<double>(val));
						break;
					case LUA_TSTRING:
						w.w_stringZ												(luabind::object_cast<LPCSTR>(val));
						break;
					default:
						FATAL													("not implemented yet");
				}
			}
			break;
		}
		default:
		{
			FATAL("not supported yet");
			return;
		}
	}
}

void CTimerCustom::load_table(IReader &r)
{
	u16								size	= r.r_u16								();
	m_argument								= luabind::newtable						(ai().script_engine().lua());
	if (size == 0)
		return;
	for (u16 i = 0; i < size; i++)
	{
		shared_str					str		= "";
		u8							type	= 0;
		r.r_stringZ																	(str);
		type								= r.r_u8								();
		switch (type)
		{
			case LUA_TBOOLEAN: 
			{
				m_argument[*str]			= r.r_bool								();
				break;
			}
			case LUA_TNUMBER:
			{
				m_argument[*str]			= r.r_double							();
				break;
			}
			case LUA_TSTRING:
			{
				shared_str			tmp		= "";
				r.r_stringZ															(tmp);
				m_argument[*str]			= *tmp;
				break;
			}
			default:
			{
				FATAL																("not implemented yet");
			}
		}
	}
}

#pragma optimize("s",on)
void CTimerCustom::script_register(lua_State *L)
{
	module(L)
		[

			class_<CTimerCustom>("Timer")
				.def(constructor<>())
				.def("SetName",						&CTimerCustom::SetName)
				.def("Name",						&CTimerCustom::Name)
				.def("SetTimerType",					&CTimerCustom::SetTimerType)
				.def("Time",						&CTimerCustom::Time)
				.def("TimeElapsed",					&CTimerCustom::TimeElapsed)
				.def("SetAction",					(void (CTimerCustom::*)(LPCSTR)) &CTimerCustom::SetAction)
				.def("Action",						&CTimerCustom::ActionScript)
				.def("SetInfo",						&CTimerCustom::SetInfo)
				.def("Info",						&CTimerCustom::InfoScript)
				.def("SetArgument",					&CTimerCustom::SetArgument)
				.def("GetArgument",					&CTimerCustom::GetArgument)

				
				.def_readwrite("day",				&CTimerCustom::m_day)
				.def_readwrite("hour",				&CTimerCustom::m_hour)
				.def_readwrite("min",				&CTimerCustom::m_min)
				.def_readwrite("sec",				&CTimerCustom::m_sec)
				.def_readwrite("ms",				&CTimerCustom::m_ms)
				.def_readwrite("game_time",			&CTimerCustom::m_game_time)

				.def("SetHUD",						&CTimerCustom::SetHUD)
				.def("isHUD",						&CTimerCustom::isHUD)
				.def("Valide",						&CTimerCustom::valide)
				.def(self == other<CTimerCustom>())
		];
}


