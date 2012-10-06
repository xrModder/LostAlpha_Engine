#pragma once
#ifndef	__KINEMATICS_H__
#define	__KINEMATICS_H__

#include "FBasicVisual.h"

class IKinematics;

typedef void (* UpdateCallback)		(IKinematics*	P);

class  CBoneData;
class IBoneData;
class  IKinematicsAnimated;
class  IRender_Visual;
class  CBoneInstance;
struct SEnumVerticesCallback;

// 10 fps
#define UCalc_Interval		(u32(100))

class ENGINE_API IKinematics
{
public:
	typedef xr_vector<std::pair<shared_str, u16> >	accel;
	struct	pick_result
	{
		Fvector	normal;
		float	dist;
		Fvector	tri[3];
	};
public:
	virtual		void					Bone_Calculate		(CBoneData* bd, Fmatrix* parent) = 0;
	virtual		void					Bone_GetAnimPos(Fmatrix& pos,u16 id, u8 channel_mask, bool ignore_callbacks) = 0;

	virtual		bool					PickBone			(const Fmatrix &parent_xform, pick_result &r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id) = 0;
	virtual		void					EnumBoneVertices	(SEnumVerticesCallback &C, u16 bone_id) = 0;

	// Low level interface
	virtual u16					__stdcall	LL_BoneID(LPCSTR  B) = 0;
	virtual u16					__stdcall	LL_BoneID(const shared_str& B) = 0;
	virtual LPCSTR				__stdcall	LL_BoneName_dbg(u16 ID) = 0;

	virtual CInifile*			__stdcall	LL_UserData() = 0;
	virtual accel*						LL_Bones() = 0;

	virtual ICF CBoneInstance&	__stdcall	LL_GetBoneInstance(u16 bone_id) = 0;

	virtual CBoneData&			__stdcall	LL_GetData(u16 bone_id) = 0;

virtual	const IBoneData&		__stdcall	GetBoneData(u16 bone_id) const = 0;

	virtual u16					__stdcall	LL_BoneCount()const = 0;
	virtual u16							LL_VisibleBoneCount() = 0;

	virtual ICF			Fmatrix& __stdcall	LL_GetTransform(u16 bone_id) = 0;
	virtual ICF const	Fmatrix& __stdcall	LL_GetTransform(u16 bone_id) const = 0;

	virtual ICF Fmatrix&				LL_GetTransform_R(u16 bone_id) = 0;
	virtual Fobb&						LL_GetBox(u16 bone_id) = 0;
	virtual const Fbox&			__stdcall	GetBox()const = 0;
	virtual void						LL_GetBindTransform(xr_vector<Fmatrix>& matrices) = 0;
	virtual int 						LL_GetBoneGroups(xr_vector<xr_vector<u16> >& groups) = 0;

	virtual u16					__stdcall	LL_GetBoneRoot() = 0;
	virtual void						LL_SetBoneRoot(u16 bone_id) = 0;

	virtual BOOL				__stdcall	LL_GetBoneVisible(u16 bone_id) = 0;
	virtual BOOL				__stdcall	LL_IsNotBoneHidden(u16 bone_id) = 0;
	virtual void						LL_SetBoneVisible(u16 bone_id, BOOL val, BOOL bRecursive) = 0;
	virtual void						LL_HideBoneVisible(u16 bone_id, BOOL bRecursive) = 0;
	virtual u64					__stdcall	LL_GetBonesVisible() = 0;
	virtual void						LL_SetBonesVisible(u64 mask) = 0;

	// Main functionality
	virtual void						CalculateBones(BOOL bForceExact	= FALSE) = 0; // Recalculate skeleton
	virtual void						CalculateBones_Invalidate() = 0;
	virtual void						Callback(UpdateCallback C, void* Param) = 0;

	//	Callback: data manipulation
	virtual void						SetUpdateCallback(UpdateCallback pCallback) = 0;
	virtual void						SetUpdateCallbackParam(void* pCallbackParam) = 0;

	virtual UpdateCallback				GetUpdateCallback() = 0;
	virtual void*						GetUpdateCallbackParam() = 0;
	//UpdateCallback						Update_Callback;
	//void*								Update_Callback_Param;
	virtual IRender_Visual*		__stdcall 	dcast_RenderVisual() = 0;
	virtual IKinematicsAnimated*	__stdcall	dcast_PKinematicsAnimated() = 0;

	// debug
#ifdef DEBUG
	virtual void						DebugRender			(Fmatrix& XFORM) = 0;
	virtual shared_str			__stdcall	getDebugName		() = 0;
#endif
};

IC IKinematics* PKinematics (IRender_Visual* V) { return V?V->dcast_PKinematics():0;}


#endif
