#pragma once

//#include "customzone.h"
#include "script_export_space.h"
#include "xr_time.h"
#include "date_time.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_time_manager.h"
#include "level.h"
#include "script_engine.h"
#include "object_broker.h"
#include "script_space_forward.h"
//#include "CustomTimersManager.h"



class CTimerCustom 
{ //: public IPureSerializeObject<IReader,IWriter> {
public:
						CTimerCustom();
	virtual				~CTimerCustom();
	

	virtual void		load				(IReader&);
	virtual void		save				(IWriter&);

	//Name
	void				SetName				(LPCSTR _name ) {m_name = _name;}
	LPCSTR				Name				(){return m_name.c_str();}

	//Time
	void				SetGameTime			();
	void				SetRealTime			();
	xrTime				Time				();
	ALife::_TIME_ID		TimeNumber			() {return m_time;};
	xrTime				TimeElapsed			();
	bool				isGameTimer			() {return m_flags.test(lmGameTimer)==1?true:false;};

	//Action
	void				SetAction			(LPCSTR _action) { m_action = _action;}
	shared_str			Action				() { return m_action;}
	LPCSTR				ActionScript		() { return m_action.c_str();}

	//Info
	void				SetInfo				(LPCSTR _info) { m_info = _info;};
	shared_str			Info				() { return m_info;}
	LPCSTR				InfoScript			() { return m_info.c_str();}
	

	//Hud
	void				SetHUD				(bool b);
	bool				isHUD				() {return m_flags.test(lmHUD)==1?true:false;}

	void				StartAction			();

	
	//valide
	bool				valide				();
	bool				CheckTime			(ALife::_TIME_ID time);

	void				SetParent			(CTimersManager *_parent) { m_parent = _parent;};

	void				PrepareTime			();
	
	bool operator == (const CTimerCustom& timer){
		return (m_name==timer.m_name);
	}

	u32					m_day,m_hour,m_min,m_sec,m_ms;

private:

	shared_str			m_name;
	shared_str			m_action;
	shared_str			m_info;
	ALife::_TIME_ID		m_time;

	enum lm_flags	{	
						lmHUD			= (1<<1),
						lmRestart		= (1<<2),
						lmGameTimer		= (1<<4),
					};

	flags8				m_flags;
	CTimersManager*		m_parent;
	bool				m_bGameTimer;

public:
	DECLARE_SCRIPT_REGISTER_FUNCTION
};


DEFINE_VECTOR(CTimerCustom, TIMER_CUSTOM_VECTOR, TIMER_CUSTOM_IT);

add_to_type_list(CTimerCustom)
#undef script_type_list
#define script_type_list save_type_list(CTimerCustom)