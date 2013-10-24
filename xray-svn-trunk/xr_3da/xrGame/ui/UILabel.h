#pragma once

#include "UIFrameLineWnd.h"
#include "UILines.h"

class CLAItem;
class CUIStatic;

class CUILabel : public CUIFrameLineWnd
{
	CLAItem*				m_lanim;
	float					m_lainm_start_time;

public:
	//IUISimpleWindow
	virtual void SetWidth	(float width);
	virtual void SetHeight	(float height);

    // CUIFrameLineWnd
			void			InitLabel		(Fvector2 pos, Fvector2 size);
	virtual void			Draw();
	virtual void			Update			();

	virtual void			SetText					(LPCSTR txt)				{m_text.TextItemControl()->SetText(txt);}
	virtual LPCSTR			GetText					()							{return m_text.TextItemControl()->GetText();}

	// own
	CUILabel();
			void			SetLightAnim			(LPCSTR lanim);
	CUIStatic				m_text;
protected:
	Fvector2	m_textPos;
};