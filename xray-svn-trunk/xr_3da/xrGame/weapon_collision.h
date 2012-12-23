////////////////////////////////////////////////////////////////////////////
//	Module		: weapon_collision.h
//	Created		: 12/10/2012
//	Modified	: 12/10/2012
//	Author		: lost alpha (SkyLoader)
//	Description	: weapon collision
////////////////////////////////////////////////////////////////////////////
#pragma once

class CWeaponCollision
{
	public:
		CWeaponCollision();
		virtual ~CWeaponCollision();
		void Load();
		void Update(Fmatrix &o, float range, bool is_zoom);
		void CheckState();

	private:
		float	fReminderDist;
		float	fReminderNeedDist;
		float	fReminderStrafe;
		float	fReminderNeedStrafe;
		bool	bFirstUpdate;
		u32	dwMState;
		bool	is_limping;
};