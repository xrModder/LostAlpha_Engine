#include "stdafx.h"
#include "HudItem.h"
#include "script_game_object.h"
#include "pch_script.h"
#include "script_callback_ex.h"
#include "actor.h"

void CHudItem::OnAnimationEnd(u32 state)
{
	CActor* actor = 0;
	Msg("%d CHudItem::OnAnimationEnd %d %s", Device.dwTimeGlobal, state, object().cName().c_str());
	if ((actor = smart_cast<CActor*>(m_object->H_Parent())) != NULL)
	{
		actor->callback(GameObject::eHudAnimCompleted)(m_object->lua_game_object(), state);
		Msg("OnAnimationEnd callback called");
	}
} 

void CHudItem::OnAnimationStart(u32 state)
{
	CActor* actor = 0;
	Msg("%d CHudItem::OnAnimationStart %d %s", Device.dwTimeGlobal, state, object().cName().c_str());
	if ((actor = smart_cast<CActor*>(m_object->H_Parent())) != NULL)
	{
		actor->callback(GameObject::eHudAnimStarted)(m_object->lua_game_object(), state);
		Msg("OnAnimationStart callback called");
	}
} 
