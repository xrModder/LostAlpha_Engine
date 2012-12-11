#include "stdafx.h"
#include "CustomTimersManager.h"
#include "ui\uistatic.h"

CTimersManager::CTimersManager(void) 
{
	b_HUDTimerActive = false;
	hud_timer = NULL;
}

CTimersManager::~CTimersManager(void) 
{
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

bool CTimersManager::AddTimer (CTimerCustom timer)
{
	if (xr_strcmp(timer.Name(),"@")) {
		if (TimerExist(timer.Name())) {
			Msg("Trying to add timer with the same timer name");
			return false;
		}
	}

	timer.PrepareTime();
	timer.SetParent(this);
	if (timer.isHUD()) OnHud(&objects.back(),true);
	objects.push_back(timer);
	return true;
}

void CTimersManager::RemoveTimer (LPCSTR name)
{
	VECTOR_TIMERS_ITERATOR it = objects.begin();
	VECTOR_TIMERS_ITERATOR it_end = objects.end();
	
	for (;it!=it_end;++it) {
		if (xr_strcmp((*it).Name(),name)==0) {
			break;
		}
	}
	R_ASSERT3(it!=it_end,"Can't find find timer with name ",name);
	
	if ((*it).isHUD()) OnHud(NULL,false);
	objects.erase(it);
}

CTimerCustom* CTimersManager::SearchTimer(LPCSTR name)
{
	VECTOR_TIMERS_ITERATOR it = objects.begin();
	VECTOR_TIMERS_ITERATOR it_end = objects.end();
	
	for (;it!=it_end;++it) {
		if (xr_strcmp((*it).Name(),name)==0) {
			return (&(*it));
		}
	}
	return NULL;
}

CTimerCustom* CTimersManager::GetTimerByName(LPCSTR name)
{
	CTimerCustom* timer = SearchTimer(name);

	R_ASSERT3(timer,"Can't find find timer with name ",name);
	return timer;
}


bool CTimersManager::TimerExist(LPCSTR name)
{
	return SearchTimer(name)!=NULL;
}

void CTimersManager::save(IWriter &memory_stream)
{
	memory_stream.open_chunk	(TIMERS_CHUNK_DATA);
	memory_stream.w_u32(objects.size());
	for (u32 idx=0;idx<objects.size();idx++) {
		objects[idx].save(memory_stream);
	}

	memory_stream.close_chunk	();
}

void CTimersManager::load(IReader &file_stream)
{
	Msg							("* Loading timers...");
	R_ASSERT2					(file_stream.find_chunk(TIMERS_CHUNK_DATA),"Can't find chunk OBJECT_CHUNK_DATA!");
	u32 size = file_stream.r_u32();
	for (u32 idx=0;idx<size;idx++) {
		CTimerCustom* timer = xr_new<CTimerCustom>();
		timer->SetParent(this);
		timer->load(file_stream);
		objects.push_back((*timer));
		//objects[idx].save(memory_stream);
	}
	Msg							("%d timers successfully loaded",size);
}


void CTimersManager::Update ()
{
	TIMER_CUSTOM_IT it = objects.begin();
	TIMER_CUSTOM_IT it_end = objects.end();

	ALife::_TIME_ID time_now = ai().get_alife() ? ai().alife().time().game_time() : Level().GetGameTime();

	objects_to_call.clear();

	for (;it!=it_end;++it) {
		if ((*it).CheckTime(time_now)) {
			if ((*it).isHUD()) {
				OnHud(NULL,false);
			}
			//(*it).StartAction();
			objects_to_call.push_back((*it));
			objects.erase(it);
		}
	}
	
	for (it=objects_to_call.begin();it!=objects_to_call.end();++it) {
		(*it).StartAction();
	}

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

