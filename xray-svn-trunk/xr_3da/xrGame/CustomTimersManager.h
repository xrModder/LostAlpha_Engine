#pragma once

#include "script_export_space.h"
#include "CustomTimer.h"
//#include "alife_registry_wrappers.h"
#include "ui.h"
#include "HudManager.h"
#include "UIGameSP.h"

class CTimersManager : public IPureSerializeObject<IReader, IWriter>
{
	typedef xr_vector<CTimerCustom>::iterator VECTOR_TIMERS_ITERATOR;
public:
					CTimersManager			(void);
	virtual			~CTimersManager			(void);

	void			Update					();

	virtual void	save					(IWriter &memory_stream);
	virtual void	load					(IReader &file_stream);

	bool			AddTimer				(CTimerCustom timer);
	void			RemoveTimer				(LPCSTR name);
	CTimerCustom*	GetTimerByName			(LPCSTR name);
	bool			TimerExist				(LPCSTR name);

	void			OnHud					(CTimerCustom *t,bool b);
	bool			IsAnyHUDTimerActive		() {return b_HUDTimerActive;};

	void				GameLoaded			(bool val) { b_GameLoaded = val;}
	bool				IsGameLoaded		() const {return b_GameLoaded;};

private:
	xr_vector<CTimerCustom> objects_to_load;
	xr_vector<CTimerCustom> objects;
	xr_vector<CTimerCustom> objects_to_call;
	CTimerCustom			*hud_timer;
	CUIStatic				*ui_hud_timer;
	bool					b_HUDTimerActive;
	bool					b_GameLoaded;
	CTimerCustom*	SearchTimer				(LPCSTR name);

public:
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CTimersManager)
#undef script_type_list
#define script_type_list save_type_list(CTimersManager)