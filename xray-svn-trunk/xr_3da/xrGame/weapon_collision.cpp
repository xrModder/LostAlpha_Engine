////////////////////////////////////////////////////////////////////////////
//	Module		: weapon_collision.cpp
//	Created		: 12/10/2012
//	Modified 	: 17/12/2012
//	Author		: lost alpha (SkyLoader)
//	Description	: weapon HUD collision (based on weapon_bobbing.cpp)
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "weapon_collision.h"
#include "actor.h"

CWeaponCollision::CWeaponCollision()
{
	Load();
}

CWeaponCollision::~CWeaponCollision()
{
}

void CWeaponCollision::Load()
{
	is_limping		= false;
	fReminderDist		= 0;
	fReminderNeedDist	= 0;
	bFirstUpdate		= true;
	fReminderStrafe		= 0;
	fReminderNeedStrafe	= 0;
}

void CWeaponCollision::CheckState()
{
	dwMState		= Actor()->MovingState();
	is_limping		= Actor()->IsLimping();
}

static const float SPEED_REMINDER = 1.f;
static const float SPEED_REMINDER_STRAFE = 0.5f;
static const float STRAFE_ANGLE = 0.15f;

void CWeaponCollision::Update(Fmatrix &o, float range, bool is_zoom)
{
	CheckState();

	Fvector	xyz	= o.c;
	Fvector	dir;
	o.getHPB(dir.x,dir.y,dir.z);

	//////collision of weapon hud

	if (bFirstUpdate) {
		fReminderDist		= xyz.z;
		fReminderNeedDist	= xyz.z;
		bFirstUpdate		= false;
		fReminderStrafe		= dir.z;
		fReminderNeedStrafe	= dir.z;
	}

	if (range < 0.8f && !is_zoom)
		fReminderNeedDist	= xyz.z - ((1 - range - 0.2) * 0.6);
	else
		fReminderNeedDist	= xyz.z;

	if (!fsimilar(fReminderDist, fReminderNeedDist)) {
		if (fReminderDist < fReminderNeedDist) {
			fReminderDist += SPEED_REMINDER * Device.fTimeDelta;
			if (fReminderDist > fReminderNeedDist)
				fReminderDist = fReminderNeedDist;
		} else if (fReminderDist > fReminderNeedDist) {
			fReminderDist -= SPEED_REMINDER * Device.fTimeDelta;
			if (fReminderDist < fReminderNeedDist)
				fReminderDist = fReminderNeedDist;
		}
	}

	if (!fsimilar(fReminderDist, xyz.z))
	{
		xyz.z 		= fReminderDist;
		o.c.set(xyz);
	}

	//////strafe inertion

	if (dwMState&ACTOR_DEFS::mcLStrafe || dwMState&ACTOR_DEFS::mcRStrafe)
	{
		float k	= ((dwMState & ACTOR_DEFS::mcCrouch) ? 0.5f : 1.f);
		if (dwMState&ACTOR_DEFS::mcLStrafe)
			k *= -1;
		if (is_zoom) k = 0.f;

		if (isActorAccelerated(dwMState, is_zoom))
			fReminderNeedStrafe	= dir.z + (STRAFE_ANGLE * k * 0.9f);
		else if (is_limping)
			fReminderNeedStrafe	= dir.z + (STRAFE_ANGLE * k * 0.75f);
		else
			fReminderNeedStrafe	= dir.z + (STRAFE_ANGLE * k);

	} else fReminderNeedStrafe = 0.f;

	if (!fsimilar(fReminderStrafe, fReminderNeedStrafe)) {
		if (fReminderStrafe < fReminderNeedStrafe)
		{
			fReminderStrafe += SPEED_REMINDER_STRAFE * Device.fTimeDelta;
			if (fReminderStrafe > fReminderNeedStrafe)
				fReminderStrafe = fReminderNeedStrafe;
		} else if (fReminderStrafe > fReminderNeedStrafe)
		{
			fReminderStrafe -= SPEED_REMINDER_STRAFE * Device.fTimeDelta;
			if (fReminderStrafe < fReminderNeedStrafe)
				fReminderStrafe = fReminderNeedStrafe;
		}
	}

	if (!fsimilar(fReminderStrafe, dir.z))
	{
		dir.z 		= fReminderStrafe;
		Fmatrix m;
		m.setHPB(dir.x,dir.y,dir.z);
		o.mul_43 (o,m);
	}

}