#pragma once

#include "UIFrameLine.h"
#include "UIWindow.h"
#include "UIStatic.h"

class CUIFrameLineWnd: public CUIWindow
{
	typedef CUIWindow inherited;
public:
	CUIFrameLineWnd();
	virtual void Init(float x, float y, float width, float height);
	virtual void Init(LPCSTR base_name, float x, float y, float width, float height, bool horizontal = true);
	virtual void InitTexture(LPCSTR tex_name, bool horizontal = true);
	virtual void Draw();
	virtual void SetWidth(float width);
	virtual void SetHeight(float height);
	virtual void SetOrientation(bool horizontal);
			float GetTextureHeight();
	void SetColor(u32 cl);

	// Also we can display textual caption on the frame
	CUIStatic		UITitleText;
	CUIStatic*		GetTitleStatic(){return &UITitleText;};

	void		SetStretchTexture			(bool stretch_texture)	{m_bStretchTexture = stretch_texture;}
	bool		GetStretchTexture			()						{return m_bStretchTexture;}

protected:
	bool			bHorizontal;
	bool			m_bStretchTexture;
	bool			m_bTextureAvailable;
	CUIFrameLine	UIFrameLine;	
};
