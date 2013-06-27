#pragma once

#include "../CustomHUD.h"
#include "HitMarker.h"
#include "UI.h"

class CContextMenu;
class CHUDTarget;

class CHUDManager :
	public CCustomHUD
{
	friend class CUI;
private:
	CUI*					pUI;
	CUIGameCustom*			pUIGame;
	CHitMarker				HitMarker;
	CHUDTarget*				m_pHUDTarget;
	bool					b_online;
public:
							CHUDManager			();
	virtual					~CHUDManager		();
	virtual		void		OnEvent				(EVENT E, u64 P1, u64 P2);

	virtual void			OnScreenResolutionChanged();
	virtual		void		Load				();
	
	virtual		void		Render_First		();
	virtual		void		Render_Last			();	   
	virtual		void		OnFrame				();

	virtual		void		RenderUI			();

	virtual		IC CUI*		GetUI				(){return pUI;}
		CUIGameCustom*		GetGameUI			(){return pUIGame;}

				void		Hit					(int idx, float power, const Fvector& dir);
	//CFontManager&			Font				()							{return *(UI().Font());}
	//текущий предмет на который смотрит HUD
	collide::rq_result&		GetCurrentRayQuery	();


	//устанвка внешнего вида прицела в зависимости от текущей дисперсии
	void					SetCrosshairDisp	(float dispf, float disps = 0.f);
	void					ShowCrosshair		(bool show);

	void					SetHitmarkType		(LPCSTR tex_name);
	virtual void			OnScreenRatioChanged();
	virtual void			OnDisconnected		();
	virtual void			OnConnected			();

	void			RenderActiveItemUI	();
	bool			RenderActiveItemUIQuery();

	virtual void			net_Relcase			(CObject *object);
};

IC CHUDManager&			HUD()		{ return *((CHUDManager*)g_hud);}