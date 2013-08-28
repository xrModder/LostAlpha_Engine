#pragma once

#include "zombie_choke_execute.h"
#include "../../../clsid_game.h"
#include "../../../entity_alive.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateZombieChokeAbstract CStateZombieChoke<_Object>

#define MAX_DISTANCE_TO_ENEMY		1.6f

TEMPLATE_SPECIALIZATION


CStateZombieChokeAbstract::CStateZombieChoke(_Object *obj) : inherited(obj)
{
	add_state	(eStateChoke_Execute,			xr_new<CStateZombieChokeExecute<_Object> >	(obj));
}

TEMPLATE_SPECIALIZATION
void CStateZombieChokeAbstract::reinit()
{
	inherited::reinit	();
	
	m_time_last_choke	= 0;
}

TEMPLATE_SPECIALIZATION
void CStateZombieChokeAbstract::initialize()
{
	inherited::initialize						();

	m_enemy	= object->EnemyMan.get_enemy		();
}

TEMPLATE_SPECIALIZATION
void CStateZombieChokeAbstract::reselect_state()
{
	if (get_state(eStateChoke_Execute)->check_start_conditions())
		select_state(eStateChoke_Execute);
	
	//ctd. here !!!!
}

TEMPLATE_SPECIALIZATION
void CStateZombieChokeAbstract::check_force_state()
{
	// check if we can start execute
	if (prev_substate == eStateChoke_Execute)
	{
		if (get_state(eStateChoke_Execute)->check_start_conditions())
			current_substate = u32(-1);
	}
}

TEMPLATE_SPECIALIZATION
void CStateZombieChokeAbstract::finalize()
{
	inherited::finalize();

	m_time_last_choke				= Device.dwTimeGlobal;
}

TEMPLATE_SPECIALIZATION
void CStateZombieChokeAbstract::critical_finalize()
{
	inherited::critical_finalize	();

	m_time_last_choke				= Device.dwTimeGlobal;
}

TEMPLATE_SPECIALIZATION
bool CStateZombieChokeAbstract::check_start_conditions()
{
	if (!object->WantChoke()) return false;

	const CEntityAlive *m_enemy = object->EnemyMan.get_enemy();
	if (m_enemy->CLS_ID != CLSID_OBJECT_ACTOR)							return false;
	if (!object->EnemyMan.see_enemy_now())								return false;
	if (object->CControlledActor::is_installed() && object->CControlledActor::is_controlling())	return false;

	const CActor *m_actor = smart_cast<const CActor*>(m_enemy);
	VERIFY(m_actor);
	if (m_actor->input_external_handler_installed())						return false;

	float dist_to_enemy = object->EnemyMan.get_enemy_position().distance_to(object->Position());
	if (dist_to_enemy > MAX_DISTANCE_TO_ENEMY)							return false;

	if (controlling_value == 1)									return false;

	if (!get_state(eStateChoke_Execute)->check_start_conditions())					return false;

	return true;
  }

TEMPLATE_SPECIALIZATION
bool CStateZombieChokeAbstract::check_completion()
{
	// если враг изменился
	if (m_enemy != object->EnemyMan.get_enemy())		return true;
	
	// если актера уже душит второй зомби
	if ((current_substate != eStateChoke_Execute) && 
		object->CControlledActor::is_controlling())	return true;

	return false;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateZombieChokeAbstract

