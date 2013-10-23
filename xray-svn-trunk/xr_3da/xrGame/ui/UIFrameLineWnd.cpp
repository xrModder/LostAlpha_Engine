#include "stdafx.h"
#include "UIFrameLineWnd.h"
#include "UIFrameLine.h"

CUIFrameLineWnd::CUIFrameLineWnd()
	:	bHorizontal(true),
		m_bTextureAvailable(false),
		m_bStretchTexture(false)
{AttachChild(&UITitleText);}

void CUIFrameLineWnd::SetWndPos(const Fvector2& pos)
{
	InitFrameLineWnd(pos,GetWndSize(),bHorizontal);
}

void CUIFrameLineWnd::SetWndSize(const Fvector2& size)
{
	InitFrameLineWnd(GetWndPos(),size,bHorizontal);
}

void CUIFrameLineWnd::InitFrameLineWnd(LPCSTR base_name, Fvector2 pos, Fvector2 size, bool horizontal)
{
	InitFrameLineWnd(pos,size,horizontal);
	InitTexture(base_name, horizontal);
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
		UITitleText.SetWndPos					(Fvector2().set(0.f,0.f));
		UITitleText.SetWndSize					(Fvector2().set(rect.right - rect.left, 50.f)); 
	}
	else
	{
		UIFrameLine.InitFrameLine(rect.lt, rect.bottom - rect.top, horizontal, alNone);
		UITitleText.SetWndPos					(Fvector2().set(0.f,0.f));
		UITitleText.SetWndSize					(Fvector2().set(50.f, rect.bottom - rect.top)); 
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
	return UIFrameLine.elements[0].GetTextureRect().height();
}

void CUIFrameLineWnd::SetOrientation(bool horizontal)
{
	UIFrameLine.SetOrientation(horizontal);
}

void CUIFrameLineWnd::SetTextureColor(u32 cl)
{
	UIFrameLine.SetTextureColor(cl);
}

static Fvector2 pt_offset		= {-0.5f, -0.5f};

void draw_rect(Fvector2 LTp, Fvector2 RBp, Fvector2 LTt, Fvector2 RBt, u32 clr, Fvector2 const& ts)
{
	UI().AlignPixel			(LTp.x);
	UI().AlignPixel			(LTp.y);
	LTp.add					(pt_offset);
	UI().AlignPixel			(RBp.x);
	UI().AlignPixel			(RBp.y);
	RBp.add					(pt_offset);
	LTt.div					(ts);
	RBt.div					(ts);

	UIRender->PushPoint(LTp.x, LTp.y,	0, clr, LTt.x, LTt.y);
	UIRender->PushPoint(RBp.x, RBp.y,	0, clr, RBt.x, RBt.y);
	UIRender->PushPoint(LTp.x, RBp.y,	0, clr, LTt.x, RBt.y);

	UIRender->PushPoint(LTp.x, LTp.y,	0, clr, LTt.x, LTt.y);
	UIRender->PushPoint(RBp.x, LTp.y,	0, clr, RBt.x, LTt.y);
	UIRender->PushPoint(RBp.x, RBp.y,	0, clr, RBt.x, RBt.y);
}

