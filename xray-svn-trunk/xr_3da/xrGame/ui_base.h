#pragma once

#include "ui_defs.h"

#define UI_BASE_WIDTH	1024.0f
#define UI_BASE_HEIGHT	768.0f

struct CFontManager;
class CUICursor;
class CUIGameCustom;

class CDeviceResetNotifier :public pureDeviceReset
{
public:
						CDeviceResetNotifier					()	{Device.seqDeviceReset.Add(this,REG_PRIORITY_NORMAL);};
	virtual				~CDeviceResetNotifier					()	{Device.seqDeviceReset.Remove(this);};
	virtual void		OnDeviceReset							()	{};

};

struct CFontManager :public pureDeviceReset			{
	CFontManager			();
	~CFontManager			();

	typedef xr_vector<CGameFont**>					FONTS_VEC;
	typedef FONTS_VEC::iterator						FONTS_VEC_IT;
	FONTS_VEC				m_all_fonts;
	void					Render					();

	// hud font
	CGameFont*				pFontMedium;
	CGameFont*				pFontDI;

	CGameFont*				pFontArial14;
	CGameFont*				pFontGraffiti19Russian;
	CGameFont*				pFontGraffiti22Russian;
	CGameFont*				pFontLetterica16Russian;
	CGameFont*				pFontLetterica18Russian;
	CGameFont*				pFontGraffiti32Russian;
	CGameFont*				pFontGraffiti50Russian;
	CGameFont*				pFontLetterica25;
	CGameFont*				pFontStat;

	void					InitializeFonts			();
	void					InitializeFont			(CGameFont*& F, LPCSTR section, u32 flags = 0);
	LPCSTR					GetFontTexName			(LPCSTR section);				

	virtual void			OnDeviceReset			();
};

class ui_core: public CDeviceResetNotifier
{
	C2DFrustum		m_2DFrustum;
	C2DFrustum		m_2DFrustumPP;
	bool			m_bPostprocess;

	CFontManager*	m_pFontManager;
	CUICursor*		m_pUICursor;

	Fvector2		m_pp_scale_;
	Fvector2		m_scale_;
	Fvector2*		m_current_scale;

	IC float		ClientToScreenScaledX			(float left)				{return left * m_current_scale->x;};
	IC float		ClientToScreenScaledY			(float top)					{return top * m_current_scale->y;};
public:
	xr_stack<Frect> m_Scissors;
	
					ui_core							();
					~ui_core						();
	CFontManager&	Font							()							{return *m_pFontManager;}
	CUICursor*		GetUICursor						()							{return m_pUICursor;}

	void			ClientToScreenScaled			(Fvector2& dest, float left, float top);
	void			ClientToScreenScaled			(Fvector2& src_and_dest);
	void			ClientToScreenScaledWidth		(float& src_and_dest);
	void			ClientToScreenScaledHeight		(float& src_and_dest);

	Frect			ScreenRect						();
	const C2DFrustum& ScreenFrustum					(){return (m_bPostprocess)?m_2DFrustumPP:m_2DFrustum;}
	void			PushScissor						(const Frect& r, bool overlapped=false);
	void			PopScissor						();

	void			pp_start						();
	void			pp_stop							();
	void			RenderFont						();

	virtual void	OnDeviceReset					();
	static	bool	is_16_9_mode					();
	shared_str		get_xml_name					(LPCSTR fn);

	IUIRender::ePointType		m_currentPointType;
};

extern CUICursor*	GetUICursor						();
extern ui_core&		UI								();
extern CUIGameCustom*	CurrentGameUI				();
