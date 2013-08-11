#pragma once
#include "ui/UIStatic.h"

class CMapLocation;
class CUIXml;

class CMapSpot :public CUIStatic
{
private:
	typedef CUIStatic inherited;
	CMapLocation*			m_map_location;
	int						m_location_level;

	CUIStatic*				m_border_static;
	
	bool					m_mark_focused;

public:
		bool				m_bScale;
		Fvector2			m_scale_bounds;

	Fvector2				m_originSize;

public:
							CMapSpot						(CMapLocation*);
	virtual					~CMapSpot						();
	virtual		void		Load							(CUIXml* xml, LPCSTR path);
	CMapLocation*			MapLocation						()							{return m_map_location;}
				int			get_location_level				()							{return m_location_level;}
	virtual LPCSTR			GetHint							();
	virtual		void		SetWndPos						(const Fvector2& pos);
	virtual		void		Update							();
	virtual		bool		OnMouseDown						(int mouse_btn);
	virtual		void		OnFocusLost						();

				void		show_static_border				(bool status);
				void		mark_focused					();
};


class CMapSpotPointer :public CMapSpot
{
	typedef CMapSpot inherited;
	xr_string				m_pointer_hint;
public:
							CMapSpotPointer					(CMapLocation*);
	virtual					~CMapSpotPointer				();
	virtual		LPCSTR		GetHint							();
};

class CMiniMapSpot :public CMapSpot
{
	typedef CMapSpot inherited;
	ui_shader				m_icon_above,m_icon_normal,m_icon_below;
	Frect					m_tex_rect_above,m_tex_rect_normal,m_tex_rect_below;
public:
							CMiniMapSpot					(CMapLocation*);
	virtual					~CMiniMapSpot					();
	virtual		void		Load							(CUIXml* xml, LPCSTR path);
	virtual		void		Draw							();
};
