#ifndef skeleton_motion_defs_inluded
#define skeleton_motion_defs_inluded

#pragma once

#define	MAX_PARTS			u32(4)

#define SAMPLE_FPS			f32(30.f)
#define SAMPLE_SPF			f32(1.f/SAMPLE_FPS)
#define	END_EPS				f32(SAMPLE_SPF+EPS)
#define KEY_Quant			f32(32767.f)
#define	KEY_QuantI			f32(1.f/KEY_Quant)

#endif
