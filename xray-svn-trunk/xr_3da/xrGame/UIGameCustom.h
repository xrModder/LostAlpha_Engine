#ifndef __XR_UIGAMECUSTOM_H__
#define __XR_UIGAMECUSTOM_H__
#pragma once

#include "script_export_space.h"
#include "object_interfaces.h"
#include "inventory_space.h"
//#include "gametype_chooser.h"
#include "UIDialogHolder.h"
#include "../CustomHUD.h"
// refs
class CUI;
class CTeamBaseZone;
class game_cl_GameState;
class CUIDialogWnd;
class CUICaption;
class CUIStatic;
class CUIWindow;
class CUIXml;			
struct KillMessageStruct;
enum EGameTypes;

struct SDrawStaticStruct :public IPureDestroyableObject
{
	SDrawStaticStruct	();
	virtual	void	destroy			();
	CUIStatic*		m_static;
	float			m_endTime;
	shared_str		m_name;
	void			Draw();
	void			Update();
	CUIStatic*		wnd()		{return m_static;}
	bool			IsActual();
	bool operator ==(LPCSTR str){
		return (m_name == str);
	}
};

typedef xr_vector<SDrawStaticStruct>	st_vec;
struct SGameTypeMaps
{
	shared_str				m_game_type_name;
	EGameTypes				m_game_type_id;
	struct SMapItm{
		shared_str	map_name;
		shared_str	map_ver;
		bool operator ==(const SMapItm& other){return map_name==other.map_name && map_ver==other.map_ver;}
	};
	xr_vector<SMapItm>		m_map_names;
};

struct SGameWeathers
{
	shared_str				m_weather_name;
	shared_str				m_start_time;
};
typedef xr_vector<SGameWeathers>					GAME_WEATHERS;
typedef xr_vector<SGameWeathers>::iterator			GAME_WEATHERS_IT;
typedef xr_vector<SGameWeathers>::const_iterator	GAME_WEATHERS_CIT;

class CMapListHelper
{
	typedef xr_vector<SGameTypeMaps>	TSTORAGE;
	typedef TSTORAGE::iterator			TSTORAGE_IT;
	typedef TSTORAGE::iterator			TSTORAGE_CIT;
	TSTORAGE							m_storage;
	GAME_WEATHERS						m_weathers;

	void						Load			();
	SGameTypeMaps*				GetMapListInt	(const shared_str& game_type);
public:
	const SGameTypeMaps&		GetMapListFor	(const shared_str& game_type);
	const SGameTypeMaps&		GetMapListFor	(const EGameTypes game_id);
	const GAME_WEATHERS&		GetGameWeathers	();
};

extern CMapListHelper	gMapListHelper;

class CUIGameCustom :public DLL_Pure, public CDialogHolder
{
protected:
	CUIWindow*			m_window;
	CUIXml*				m_msgs_xml;

	st_vec									m_custom_statics;

public:
	virtual	void		shedule_Update			(u32 dt) {};
	virtual void		SetClGame				(game_cl_GameState* g);
	
						CUIGameCustom			();
	virtual				~CUIGameCustom			();

	virtual	void		Init					(int stage)	{};
	
	virtual void		Render					();
	virtual void _BCL	OnFrame					();

	
	virtual void		HideShownDialogs		(){};

	SDrawStaticStruct*	AddCustomStatic			(LPCSTR id, bool bSingleInstance);
	SDrawStaticStruct*	GetCustomStatic			(LPCSTR id);
	void				RemoveCustomStatic		(LPCSTR id);

	virtual void		ChangeTotalMoneyIndicator(LPCSTR newMoneyString)		{};
	virtual void		DisplayMoneyChange		(LPCSTR deltaMoney)			{};
	virtual void		DisplayMoneyBonus		(KillMessageStruct* bonus)	{};
	
	virtual void		UnLoad					();
	void				Load					();

	virtual	void					reset_ui				();

	DECLARE_SCRIPT_REGISTER_FUNCTION
}; // class CUIGameCustom
extern CUIGameCustom*		CurrentGameUI();
#endif