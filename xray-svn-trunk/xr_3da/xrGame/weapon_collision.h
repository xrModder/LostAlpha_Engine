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

	private:
		float	fTime;
		float	fReminderDist;
		float	fReminderNeedDist;
		bool	bFirstUpdate;
};