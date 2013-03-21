#include "stdafx.h"
#include "CustomTimersManager.h"
#include "ui\uistatic.h"

CTimersManager::CTimersManager() 
{
	b_HUDTimerActive = false;
	hud_timer = NULL;
	b_GameLoaded = false;
}

CTimersManager::~CTimersManager() 
{
	delete_data					(objects);
}

void CTimersManager::OnHud(CTimerCustom *t,bool b)
{
	if (b) {
		if (!t) return;
		if (IsAnyHUDTimerActive()) {
			FATAL("Trying to add more than one HUD timer");
		} else {
			b_HUDTimerActive = true;
			ui_hud_timer = HUD().GetUI()->UIGame()->AddCustomStatic("hud_timer", true)->wnd();
			hud_timer = t;
		}
	} else {
		if (IsAnyHUDTimerActive()) {
			HUD().GetUI()->UIGame()->RemoveCustomStatic("hud_timer");
			ui_hud_timer = NULL;
			b_HUDTimerActive = false;
			hud_timer = NULL;
		}
	}
}

bool CTimersManager::AddTimer (CTimerCustom *timer)
{
	if (xr_strcmp(timer->Name(),"@")) {
		if (TimerExist(timer->Name())) {
			Msg("Trying to add timer with the same timer name");
			return false;
		}
	}

	if (timer->isGameTimer() && !timer->isHUD())
		timer->PrepareGameTime();
	else
		timer->PrepareTime();
	timer->SetParent(this);
	if (timer->isHUD()) 
		OnHud(objects.back(),true);
	objects.push_back(timer);
	return true;
}

void CTimersManager::RemoveTimer (LPCSTR name)
{
	TIMERS_IT it = std::find_if(objects.begin(), objects.end(), STimerPred(name));
	
	R_ASSERT3(it!=objects.end(),"Can't find timer with name ",name);
	
	if ((*it)->isHUD()) 
		OnHud(NULL,false);
	objects.erase(it);
	xr_delete(*it);
}

CTimerCustom* CTimersManager::SearchTimer(LPCSTR name)
{
	TIMERS_IT it = std::find_if(objects.begin(), objects.end(), STimerPred(name));
	if (it != objects.end())
		return *it;
	return NULL;
}

CTimerCustom* CTimersManager::GetTimerByName(LPCSTR name)
{
	CTimerCustom* timer = SearchTimer(name);
	R_ASSERT3(timer,"Can't find timer with name ",name);
	return timer;
}


bool CTimersManager::TimerExist(LPCSTR name) 
{
	return SearchTimer(name)!=NULL;
}

void CTimersManager::save(IWriter &memory_stream)
{
	Msg							("* Saving timers...");

	memory_stream.open_chunk	(TIMERS_CHUNK_DATA);
	memory_stream.w_u16(objects.size());

	TIMERS_IT it = objects.begin();
	TIMERS_IT it_end = objects.end();

	for (;it!=it_end;++it) {
		(*it)->save(memory_stream);
	}

	memory_stream.close_chunk	();
	Msg							("* %d timers successfully saved",objects.size());
}

void CTimersManager::load(IReader &file_stream)
{
	Msg							("* Loading timers...");
	R_ASSERT2					(file_stream.find_chunk(TIMERS_CHUNK_DATA),"Can't find chunk TIMERS_CHUNK_DATA!");
	u16 size = file_stream.r_u16();
	for (int idx=0;idx<size;idx++) {
		CTimerCustom* timer = xr_new<CTimerCustom>();
		timer->SetParent(this);
		timer->load(file_stream);
		objects_to_load.push_back(timer);
	}
	Msg							("* %d timers successfully loaded",size);
}


void CTimersManager::Update ()
{
	if (!b_GameLoaded)
		return;
	TIMERS_IT it = objects.begin();
	TIMERS_IT it_end = objects.end();

	objects_to_call.clear();

	for (;it!=it_end;++it) 
	{
		if ((*it)->isGameTimer() && !(*it)->isHUD())
		{
			if ((*it)->CheckGameTime())
			{
				objects_to_call.push_back((*it));
				objects.erase(it);
				break;
			}
		} else {
			ALife::_TIME_ID timer_time = ai().get_alife() ? ai().alife().time().game_time() : Level().GetGameTime();

			if ((*it)->CheckTime(timer_time)) 
			{
				if ((*it)->isHUD())
					OnHud(NULL,false);
				objects_to_call.push_back((*it));
				objects.erase(it);
				break;
			}
		}
	}

	if (b_GameLoaded)
	{
		for (it=objects_to_load.begin();it!=objects_to_load.end();++it)
		{
			if ((*it)->isGameTimer() && !(*it)->isHUD())
				(*it)->PrepareGameTime();
			else
				(*it)->PrepareTime();
	
			objects.push_back((*it));
		}

		objects_to_load.clear();
	}
	
	for (it=objects_to_call.begin();it!=objects_to_call.end();++it)
		(*it)->StartAction();

	ALife::_TIME_ID time_now = ai().get_alife() ? ai().alife().time().game_time() : Level().GetGameTime();

	if (hud_timer) {
		string64 str;

		ALife::_TIME_ID time_elapsed = hud_timer->TimeNumber() - time_now;

		u32 _years,_months,_days,_hours=0,_minutes=0,_seconds=0,_mseconds;
		split_time(time_elapsed,_years,_months,_days,_hours,_minutes,_seconds,_mseconds);

		if (hud_timer->isGameTimer())
			sprintf(str,"%02d:%02d",_hours,_minutes);
		else
			sprintf(str,"%02d:%02d",_minutes,_seconds);

		ui_hud_timer->SetText(str);
	}
}

