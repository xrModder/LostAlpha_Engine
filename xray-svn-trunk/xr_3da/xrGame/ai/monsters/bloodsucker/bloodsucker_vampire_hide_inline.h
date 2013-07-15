#pragma once
#include "../states/state_hide_from_point.h"
#include "bloodsucker_predator.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateBloodsuckerVampireHideAbstract CStateBloodsuckerVampireHide<_Object>

TEMPLATE_SPECIALIZATION
CStateBloodsuckerVampireHideAbstract::CStateBloodsuckerVampireHide(_Object *obj) : inherited(obj)
{
	add_state	(eStateVampire_RunAway,		xr_new<CStateMonsterHideFromPoint<_Object> >	(obj));
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireHideAbstract::reselect_state()
{
	select_state(eStateVampire_RunAway);
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireHideAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eStateVampire_RunAway) {
		SStateHideFromPoint		data;
		data.point				= object->EnemyMan.get_enemy_position();
		data.accelerated		= true;
		data.braking			= false;
		data.accel_type			= eAT_Aggressive;
		data.distance			= object->m_vampire_runaway_distance;
		data.action.action		= ACT_RUN;
		data.action.sound_type	= MonsterSound::eMonsterSoundAggressive;
		data.action.sound_delay = object->db().m_dwAttackSndDelay;
		data.action.time_out	= object->m_vampire_runaway_time;

		state->fill_data_with(&data, sizeof(SStateHideFromPoint));

		return;
	}
}

TEMPLATE_SPECIALIZATION
bool CStateBloodsuckerVampireHideAbstract::check_completion()
{
	if ((current_substate == eStateVampire_RunAway) && 
		get_state_current()->check_completion())	return true;
	
	return false;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateBloodsuckerVampireHideAbstract
