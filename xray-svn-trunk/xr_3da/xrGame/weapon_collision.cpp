////////////////////////////////////////////////////////////////////////////
//	Module		: weapon_collision.cpp
//	Created		: 12/10/2012
//	Modified 	: 13/01/2013
//	Author		: lost alpha (SkyLoader)
//	Description	: weapon HUD collision
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
	fReminderStrafe		= xr_new<CEnvelope>();
	fReminderNeedStrafe	= 0;
}

void CWeaponCollision::CheckState()
{
	dwMState		= Actor()->MovingState();
	is_limping		= Actor()->IsLimping();
}

static const float SPEED_REMINDER = 1.f;
static const u16 TIME_REMINDER_STRAFE = 300;
static const float STRAFE_ANGLE = 0.1f;

extern Flags32 psActorFlags;

void CWeaponCollision::Update(Fmatrix &o, float range, bool is_zoom)
{
	CheckState();

	Fvector	xyz	= o.c;
	Fvector	dir;
	o.getHPB(dir.x,dir.y,dir.z);

	//////collision of weapon hud:

	if (bFirstUpdate) {
		fReminderDist		= xyz.z;
		fReminderNeedDist	= xyz.z;
		bFirstUpdate		= false;
		fReminderNeedStrafe	= dir.z;
		fReminderStrafe->InsertKey(Device.dwTimeGlobal, dir.z);
		fNewStrafeTime	= Device.dwTimeGlobal;
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

	//////strafe inertion:

	if (!psActorFlags.test(AF_STRAFE_INERT)) return;

	float fLastStrafe = fReminderNeedStrafe;
	if ((dwMState&ACTOR_DEFS::mcLStrafe || dwMState&ACTOR_DEFS::mcRStrafe) && !is_zoom)
	{
		float k	= ((dwMState & ACTOR_DEFS::mcCrouch) ? 0.5f : 1.f);
		if (dwMState&ACTOR_DEFS::mcLStrafe)
			k *= -1.f;

		fReminderNeedStrafe	= dir.z + (STRAFE_ANGLE * k);

		if (dwMState&ACTOR_DEFS::mcFwd || dwMState&ACTOR_DEFS::mcBack)
			fReminderNeedStrafe	/= 2.f;

	} else fReminderNeedStrafe = dir.z;

	float result;
	if (fNewStrafeTime>(float)Device.dwTimeGlobal)
		result = fReminderStrafe->Evaluate(Device.dwTimeGlobal);
	else {
		if (fReminderStrafe->keys.size()>0)
			result = fReminderStrafe->Evaluate(fReminderStrafe->keys.back()->time);
	}

	if (!fsimilar(fLastStrafe, fReminderNeedStrafe))
	{
		float time_new = TIME_REMINDER_STRAFE+Device.dwTimeGlobal;

		fReminderStrafe->DeleteLastKeys(Device.dwTimeGlobal);

		if (!fsimilar(result, 0.f))
			fReminderStrafe->InsertKey(Device.dwTimeGlobal, result);
		else
			fReminderStrafe->InsertKey(Device.dwTimeGlobal, dir.z);
		fReminderStrafe->InsertKey(time_new, fReminderNeedStrafe);

		fNewStrafeTime			= time_new;

		result = fReminderStrafe->Evaluate(Device.dwTimeGlobal);
	}

	if (!fsimilar(result, dir.z))
	{
		dir.z 		= result;
		Fmatrix m;
		m.setHPB(dir.x,dir.y,dir.z);
		Fmatrix tmp;
		tmp.mul_43(o, m);
		o.set(tmp);
	} else {
		if (fReminderStrafe->keys.size()>10 && fNewStrafeTime<(float)Device.dwTimeGlobal)
		{
			fReminderStrafe->Clear();	//clear all keys
			fReminderStrafe->InsertKey(Device.dwTimeGlobal, fReminderNeedStrafe);
			fNewStrafeTime			= Device.dwTimeGlobal;
		}
	}

}