#pragma once

#include "script_export_space.h"
#include "CustomTimer.h"
//#include "alife_registry_wrappers.h"
#include "ui.h"
#include "HudManager.h"
#include "UIGameSP.h"

class CTimersManager : public IPureSerializeObject<IReader, IWriter>
{
	DEFINE_VECTOR(CTimerCustom*, TIMERS_STORAGE, TIMERS_IT);
public:
					CTimersManager			();
	virtual			~CTimersManager			();

	void			Update					();

	virtual void	save					(IWriter &memory_stream);
	virtual void	load					(IReader &file_stream);

	bool			AddTimer				(CTimerCustom *timer);
	void			RemoveTimer				(LPCSTR name);
	CTimerCustom*	GetTimerByName			(LPCSTR name);
	bool			TimerExist				(LPCSTR name);

	void			OnHud					(CTimerCustom *t,bool b);
	bool			IsAnyHUDTimerActive		() const					{ return b_HUDTimerActive; }

	void			GameLoaded				(bool val)					{ b_GameLoaded = val; }
	bool			IsGameLoaded			() const					{ return b_GameLoaded; }

private:
	TIMERS_STORAGE			objects_to_load;
	TIMERS_STORAGE			objects;
	TIMERS_STORAGE			objects_to_call;
	CTimerCustom			*hud_timer;
	CUIStatic				*ui_hud_timer;
	bool					b_HUDTimerActive;
	bool					b_GameLoaded;
private:
	CTimerCustom*	SearchTimer				(LPCSTR name);

public:
	DECLARE_SCRIPT_REGISTER_FUNCTION;
private:
	struct STimerPred
	{
		private:
			shared_str m_name;
		public:
			STimerPred(shared_str name) : m_name(name) {}
			bool operator() (const CTimerCustom &t) { return xr_strcmp(t.Name(), m_name) == 0; }
			bool operator() (CTimerCustom *t)		{ return xr_strcmp(t->Name(), m_name) == 0; }
	};
};
add_to_type_list(CTimersManager)
#undef script_type_list
#define script_type_list save_type_list(CTimersManager)