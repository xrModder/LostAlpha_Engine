#pragma once
#ifndef __ACTOR_STATE_H__
#define __ACTOR_STATE_H__

enum EActorState
{
	eHungerRed = 0,
	eHungerYellow,
	eHungerGreen,
	eHungerInactive,
	eThirstRed,
	eThirstYellow,
	eThirstGreen,
	eThirstInactive,
	eSleepRed,
	eSleepYellow,
	eSleepGreen,
	eSleepInactive,
	ePsyHealthRed,
	ePsyHealthYellow,
	ePsyHealthGreen,
	ePsyHealthInactive,
	eJammedRed,
	eJammedYellow,
	eJammedGreen,
	eJammedInactive,
	eRadiationRed,
	eRadiationYellow,
	eRadiationGreen,
	eRadiationInactive,
	eBleedingRed,
	eBleedingYellow,
	eBleedingGreen,
	eBleedingInactive,
};

#endif