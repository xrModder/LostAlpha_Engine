#ifndef KINEMATIC_DEFS_INCLUDED
#define KINEMATIC_DEFS_INCLUDED

#pragma once

#include	"SkeletonDefs.h"
// consts
#define		MAX_BLENDED			u32(16)
#define		MAX_CHANNELS		u32(4)

#define		MAX_BLENDED_POOL	u32(MAX_BLENDED*MAX_PARTS*MAX_CHANNELS)
#define		MAX_ANIM_SLOT		u32(48)
class			CBlend;
typedef svector<CBlend*,MAX_BLENDED*MAX_CHANNELS>	BlendSVec;//*MAX_CHANNELS
typedef BlendSVec::iterator							BlendSVecIt;
typedef BlendSVec::const_iterator					BlendSVecCIt;
//**********************************************************************************

#endif