#include "stdafx.h"
#include "UI.h"
#include "../xr_IOConsole.h"
#include "Entity.h"
#include "HUDManager.h"
#include "UIGameSP.h"
#include "actor.h"
#include "level.h"
#include "game_cl_base.h"
#include "ui/UIMainIngameWnd.h"
#include "ui/UIMessagesWindow.h"
#include "ui/UIPdaWnd.h"

CUI::CUI(CHUDManager* p)
{
	UIMainIngameWnd					= xr_new<CUIMainIngameWnd>	();
	UIMainIngameWnd->Init			();
	m_pMessagesWnd					= xr_new<CUIMessagesWindow>();

	m_Parent						= p;
	pUIGame							= NULL;

	ShowGameIndicators				();
	ShowCrosshair					();
}
//--------------------------------------------------------------------

CUI::~CUI()
{
	xr_delete		(m_pMessagesWnd);
	xr_delete		(pUIGame);
	xr_delete		(UIMainIngameWnd);
}

//--------------------------------------------------------------------

void CUI::Load(CUIGameCustom* pGameUI)
{
	if(pGameUI){
		pGameUI->SetClGame(&Game());
		m_pMessagesWnd->SetChatOwner(&Game());
		return;
	}
	pUIGame = Game().createGameUI();
	R_ASSERT(pUIGame);
}

void CUI::UnLoad()
{
	xr_delete		(pUIGame);
	pUIGame = Game().createGameUI();
	R_ASSERT(pUIGame);
}
//--------------------------------------------------------------------

void CUI::UIOnFrame()
{
	CEntity* m_Actor = smart_cast<CEntity*>(Level().CurrentEntity());
	if (m_Actor){
		
		//update windows
		if( GameIndicatorsShown() && psHUD_Flags.is(HUD_DRAW|HUD_DRAW_RT) )
		{
			UIMainIngameWnd->Update	();
		}
	}

	// out GAME-style depend information
	if( GameIndicatorsShown() )
	{
		if (pUIGame) pUIGame->OnFrame	();
	}
	m_pMessagesWnd->Update();
}
//--------------------------------------------------------------------
#include "inventory.h"
#include "huditem.h"
bool CUI::Render()
{
	if( GameIndicatorsShown() )
	{
		if (pUIGame) 
			pUIGame->Render	();
	}

	CEntity* pEntity = smart_cast<CEntity*>(Level().CurrentEntity());
	if (pEntity)
	{
		CActor* pActor			=	smart_cast<CActor*>(pEntity);
		if(pActor)
		{
			PIItem item		=  pActor->inventory().ActiveItem();
			if(item && pActor->HUDview() && smart_cast<CHudItem*>(item))
				(smart_cast<CHudItem*>(item))->OnDrawUI();
		}

		if( GameIndicatorsShown() && psHUD_Flags.is(HUD_DRAW | HUD_DRAW_RT) )
		{
			UIMainIngameWnd->Draw();
			m_pMessagesWnd->Draw();
		}
		else
		{  //hack - draw messagess wnd in scope mode
			CUIGameSP* gSP = smart_cast<CUIGameSP*>(CurrentGameUI());
			if (gSP){
				if (!gSP->PdaMenu->GetVisible())
					m_pMessagesWnd->Draw();
			}
			else
				m_pMessagesWnd->Draw();
		}	
	}
	else
		m_pMessagesWnd->Draw();

	DoRenderDialogs();

	return false;
}
//.		if(HUD().GetUI())HUD().GetUI()->HideGameIndicators();
//.		if(HUD().GetUI())HUD().GetUI()->ShowGameIndicators();

bool	CUI::IR_OnMouseWheel			(int direction)
{
	Fvector2 pos			= GetUICursor().GetCursorPosition();

	if ( TopInputReceiver() ){
		if( TopInputReceiver()->OnMouseAction(pos.x,pos.y,(direction>0)?WINDOW_MOUSE_WHEEL_UP:WINDOW_MOUSE_WHEEL_DOWN) )
			return true;
	}

	if (pUIGame&&pUIGame->OnMouseAction(pos.x,pos.y,(direction>0)?WINDOW_MOUSE_WHEEL_UP:WINDOW_MOUSE_WHEEL_DOWN)) 
		return true;

	if( TopInputReceiver() )
		return true;

	return false;
}

//--------------------------------------------------------------------
bool CUI::IR_OnKeyboardHold(int dik)
{
	if ( TopInputReceiver() ) {
		if( TopInputReceiver()->OnKeyboardAction(dik, WINDOW_KEY_HOLD) )
			return true;
	}

	if( TopInputReceiver() )
		return true;

	return false;
}


bool CUI::IR_OnKeyboardPress(int dik)
{

	if ( TopInputReceiver() ) {
		if( TopInputReceiver()->OnKeyboardAction(dik, WINDOW_KEY_PRESSED) )
			return true;
	}

	if(UIMainIngameWnd->OnKeyboardPress(dik))
		return true;

	if (pUIGame && pUIGame->OnKeyboardAction(dik, WINDOW_KEY_PRESSED)) 
		return true;

	if( TopInputReceiver() )
		return true;

	return false;
}
//--------------------------------------------------------------------

bool CUI::IR_OnKeyboardRelease(int dik)
{
	if ( TopInputReceiver() )
	{
		if( TopInputReceiver()->OnKeyboardAction(dik, WINDOW_KEY_RELEASED) )
			return true;
	}

	if (pUIGame&&pUIGame->OnKeyboardAction(dik, WINDOW_KEY_RELEASED)) 
		return true;

	if( TopInputReceiver() )
		return true;

	return false;
}
//--------------------------------------------------------------------

bool CUI::IR_OnMouseMove(int dx,int dy)
{
	if ( TopInputReceiver() )
	{
		if ( TopInputReceiver()->OnMouseAction(dx,dy, WINDOW_MOUSE_MOVE) )
			return true;
	}
	
	if (pUIGame&&pUIGame->OnMouseAction(dx,dy, WINDOW_MOUSE_MOVE)) 
		return true;

	if( TopInputReceiver() )
		return true;
	
	return false;
}

void CUI::CommonMessageOut(LPCSTR text)
{
	m_pMessagesWnd->AddLogMessage(text);
}

SDrawStaticStruct* CUI::AddInfoMessage			(LPCSTR message)
{
	SDrawStaticStruct* ss	=	pUIGame->GetCustomStatic(message);
	if(!ss)
	{
		ss					= pUIGame->AddCustomStatic(message, true);
	}
	return ss;
}

void CUI::ShowGameIndicators()
{
	m_bShowGameIndicators	= true;
}

void CUI::HideGameIndicators()					
{
	m_bShowGameIndicators	= false;
}

void CUI::ShowCrosshair()
{
	psHUD_Flags.set		(HUD_CROSSHAIR_RT, TRUE);
}

void CUI::HideCrosshair()
{
	psHUD_Flags.set		(HUD_CROSSHAIR_RT, FALSE);
}

bool CUI::CrosshairShown()
{
	return !!psHUD_Flags.test	(HUD_CROSSHAIR_RT);
}

void CUI::OnConnected()
{
	if(!UIMainIngameWnd)
	{
		UIMainIngameWnd = xr_new<CUIMainIngameWnd>();
		UIMainIngameWnd->Init();
		//Load();
	}

	UIMainIngameWnd->OnConnected();
}
