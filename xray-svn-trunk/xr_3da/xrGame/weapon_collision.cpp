////////////////////////////////////////////////////////////////////////////
//	Module		: weapon_collision.cpp
//	Created		: 12/10/2012
//	Modified 	: 12/10/2012
//	Author		: lost alpha (SkyLoader)
//	Description	: weapon HUD collision (based on weapon_bobbing.cpp)
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "weapon_collision.h"

CWeaponCollision::CWeaponCollision()
{
	Load();
}

CWeaponCollision::~CWeaponCollision()
{
}

void CWeaponCollision::Load()
{
	fTime			= 0;
	fReminderDist		= 0;
	fReminderNeedDist	= 0;
	bFirstUpdate		= false;
}

static const float SPEED_REMINDER = 3.f;

void CWeaponCollision::Update(Fmatrix &o, float range, bool is_zoom)
{
	Fvector	xyz	= o.c;

	if (!bFirstUpdate) {
		fReminderDist		= xyz.z;
		fReminderNeedDist	= xyz.z;
		bFirstUpdate		= true;
	}

	fTime			+= Device.fTimeDelta;

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
}