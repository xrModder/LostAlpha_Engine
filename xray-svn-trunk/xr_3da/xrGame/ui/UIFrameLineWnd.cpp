#include "stdafx.h"
#include "UIFrameLineWnd.h"

CUIFrameLineWnd::CUIFrameLineWnd()
	:	bHorizontal(true),
		m_bTextureAvailable(false),
		m_bStretchTexture(false)
{AttachChild(&UITitleText);}

void CUIFrameLineWnd::Init(LPCSTR base_name, float x, float y, float width, float height, bool horizontal)
{
	inherited::Init(x,y, width, height);

	UITitleText.Init(0,0, width, 50);

	InitTexture		(base_name, horizontal);

	Fvector2 pos, size;
	pos.x			= x;
	pos.y			= y;

	size.x			= width;
	size.y			= height;

	InitFrameLineWnd (pos,size,horizontal);
}

void CUIFrameLineWnd::Init(float x, float y, float width, float height)
{
	inherited::Init(x,y, width, height);

	UITitleText.Init(0,0, width, 50);

	Fvector2 pos, size;
	pos.x			= x;
	pos.y			= y;

	size.x			= width;
	size.y			= height;

	InitFrameLineWnd (pos,size,true); //skyloader: horizontal by default
}

void CUIFrameLineWnd::SetWndPos(const Fvector2& pos)
{
	InitFrameLineWnd(pos,GetWndSize(),bHorizontal);
}

void CUIFrameLineWnd::SetWndSize(const Fvector2& size)
{
	InitFrameLineWnd(GetWndPos(),size,bHorizontal);
}

void CUIFrameLineWnd::InitFrameLineWnd(Fvector2 pos, Fvector2 size, bool horizontal)
{
	CUIWindow::SetWndPos		(pos);
	CUIWindow::SetWndSize		(size);
	
	UIFrameLine.set_parent_wnd_size(size);
	UIFrameLine.bStretchTexture = m_bStretchTexture;

	bHorizontal = horizontal;

	Frect			rect;
	GetAbsoluteRect	(rect);

	if (horizontal)
	{
		UIFrameLine.InitFrameLine(rect.lt, rect.right - rect.left, horizontal, alNone);
		UITitleText.Init(0,0, rect.right - rect.left, 50);
	}
	else
	{
		UIFrameLine.InitFrameLine(rect.lt, rect.bottom - rect.top, horizontal, alNone);
		UITitleText.Init(0,0, 50, rect.bottom - rect.top);
	}

}

void CUIFrameLineWnd::InitTexture(LPCSTR tex_name, bool horizontal)
{
	UIFrameLine.InitTexture(tex_name, "hud\\default");

	m_bTextureAvailable = true;
}

void CUIFrameLineWnd::Draw()
{
	if (m_bTextureAvailable)
	{
		Fvector2 p;
		GetAbsolutePos		(p);
		UIFrameLine.SetPos	(p);
		UIFrameLine.Render	();
	}	
	inherited::Draw();
}


void CUIFrameLineWnd::SetWidth(float width)
{
	inherited::SetWidth(width);
	if (bHorizontal)
		UIFrameLine.SetSize(width);
}

void CUIFrameLineWnd::SetHeight(float height)
{
	inherited::SetHeight(height);
	if (!bHorizontal)
		UIFrameLine.SetSize(height);
}

float CUIFrameLineWnd::GetTextureHeight()
{
	return UIFrameLine.elements[0].GetRect().height();
}

void CUIFrameLineWnd::SetOrientation(bool horizontal)
{
	UIFrameLine.SetOrientation(horizontal);
}

void CUIFrameLineWnd::SetColor(u32 cl)
{
	UIFrameLine.SetColor(cl);
}