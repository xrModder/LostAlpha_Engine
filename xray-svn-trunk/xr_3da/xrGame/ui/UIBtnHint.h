#pragma once
#include "UIFrameWindow.h"
#include "UIWindow.h"

class CUITextWnd;

class CUIButtonHint :public CUIFrameWindow
{
public:
					CUIButtonHint	();
	virtual			~CUIButtonHint	();
	CUIWindow*		Owner			()	{return m_ownerWnd;}
	void			Discard			()	{m_ownerWnd=NULL;};
	void			OnRender		();
	void			Draw_			()	{m_enabledOnFrame = true;};
	void			SetHintText		(CUIWindow* w, LPCSTR text);

	CUIWindow*			m_ownerWnd;

	CUITextWnd*			m_text;
	bool				m_enabledOnFrame;
};

extern CUIButtonHint* g_btnHint; 
extern CUIButtonHint* g_statHint;
