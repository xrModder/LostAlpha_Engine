#include "stdafx.h"
#include "uiscrollbox.h"
#include "..\uicursor.h"

CUIScrollBox::CUIScrollBox()
{
	m_bIsHorizontal			= true;
}

void CUIScrollBox::SetHorizontal()
{
	m_bIsHorizontal = true;
}

void CUIScrollBox::SetVertical()
{
	m_bIsHorizontal = false;
}

bool CUIScrollBox::OnMouseAction(float x, float y, EUIMessages mouse_action)
{
	bool cursor_over;

	if(x>=-20.0f && x<GetWidth()+20.0f && y>=-20.0f && y<GetHeight()+20.0f)
		cursor_over = true;
	else
		cursor_over = false;

	bool im_capturer = (GetParent()->GetMouseCapturer()==this);

	if(mouse_action == WINDOW_LBUTTON_DOWN)
	{
		GetParent()->SetCapture(this, true);
	}
	else if(mouse_action == WINDOW_LBUTTON_UP)
	{		
		GetParent()->SetCapture(this, false);
	}
	else if(im_capturer && mouse_action == WINDOW_MOUSE_MOVE && cursor_over)
	{
		Fvector2	pos		= GetWndPos();
		Fvector2	delta	= GetUICursor().GetCursorPositionDelta();

		if(m_bIsHorizontal)
			pos.x				+= delta.x;
		else
			pos.y				+= delta.y;

		SetWndPos			(pos);

		GetMessageTarget()->SendMessage(this, SCROLLBOX_MOVE);
	}
	return				true;
}

void CUIScrollBox::Draw()
{
	m_UIStaticItem.SetSize		(Fvector2().set(GetWidth(), GetHeight()) );

	inherited::Draw();
}