#include "stdafx.h"
#include "UIGameMP.h"
#include "UICursor.h"
#include "Level.h"
#include "game_cl_mp.h"

UIGameMP::UIGameMP() :
	m_game(NULL)
{
}

UIGameMP::~UIGameMP()
{
}

#include <dinput.h>

bool UIGameMP::IR_UIOnKeyboardPress(int dik)
{
	if ( is_binded(kCROUCH, dik) && Level().IsDemoPlay())
	{
		return true;
	}

	return inherited::IR_UIOnKeyboardPress(dik);
}

bool UIGameMP::IR_UIOnKeyboardRelease(int dik)
{
	return inherited::IR_UIOnKeyboardRelease(dik);
}

bool UIGameMP::IsServerInfoShown	()
{
	return false;
}

//shows only if it has some info ...
bool UIGameMP::ShowServerInfo()
{
/*	if (Level().IsDemoPlay())
		return true;

	VERIFY2(m_pServerInfo, "game client UI not created");
	if (!m_pServerInfo)
	{
		return false;
	}

	if (!m_pServerInfo->HasInfo())
	{
		m_game->OnMapInfoAccept();
		return true;
	}

	if (!m_pServerInfo->IsShown())
	{
		m_pServerInfo->ShowDialog(true);
	}
*/
	return true;
}

void UIGameMP::SetClGame(game_cl_GameState* g)
{
	inherited::SetClGame(g);
	m_game = smart_cast<game_cl_mp*>(g);
	VERIFY(m_game);
}

void UIGameMP::SetServerLogo(u8 const * data_ptr, u32 data_size)
{
//	m_pServerInfo->SetServerLogo(data_ptr, data_size);
}
void UIGameMP::SetServerRules(u8 const * data_ptr, u32 data_size)
{
//	m_pServerInfo->SetServerRules(data_ptr, data_size);
}

