#pragma once

#include "UIFrameLine.h"
#include "UIWindow.h"
#include "UIStatic.h"

class CUIFrameLineWnd: public CUIWindow
{
	typedef CUIWindow inherited;
public:
				 CUIFrameLineWnd	();
	virtual void InitFrameLineWnd	(LPCSTR base_name, Fvector2 pos, Fvector2 size, bool horizontal = true);
	virtual void InitFrameLineWnd	(Fvector2 pos, Fvector2 size, bool horizontal = true);
	virtual void InitTexture		(LPCSTR tex_name, bool horizontal = true);
	virtual void Draw				();
	virtual void SetWidth			(float width);
	virtual void SetHeight			(float height);

	// Also we can display textual caption on the frame
	CUIStatic		UITitleText;
	CUIStatic*		GetTitleStatic(){return &UITitleText;};

	virtual void SetWndPos			(const Fvector2& pos);
	virtual void SetWndSize			(const Fvector2& size);

	virtual void SetOrientation		(bool horizontal);
			float GetTextureHeight	();
			void SetTextureColor			(u32 cl);
			
			void SetStretchTexture	( bool bStretch )	{ m_bStretchTexture = bStretch; }
			bool GetStretchTexture	() const			{ return m_bStretchTexture; }
			bool IsHorizontal		()			{return bHorizontal;}
			void SetHorizontal		(bool horiz)		{bHorizontal = horiz;}
protected:
	bool			bHorizontal;
	bool			m_bTextureAvailable;
	CUIFrameLine	UIFrameLine;	
	bool			m_bStretchTexture;
};
