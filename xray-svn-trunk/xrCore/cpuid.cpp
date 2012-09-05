#include "stdafx.h"
#pragma hdrstop

#include "cpuid.h"

/***
*
* int _cpuid (_p_info *pinfo)
* 
* Entry:
*
*   pinfo: pointer to _p_info.
*
* Exit:
*
*   Returns int with capablity bit set even if pinfo = NULL
*
****************************************************/

#ifndef M_BORLAND
int _cpuid ( _processor_info *pinfo )
{__asm {

	// set pointers
	mov			edi , DWORD PTR [pinfo]

	// zero result
	xor			esi , esi
	mov			BYTE PTR  [edi][_processor_info::model_name][0] , 0
	mov			BYTE PTR  [edi][_processor_info::v_name][0] , 0

	// test for CPUID presence
	pushfd
	pop			eax
	mov			ebx , eax
	xor			eax , 00200000h
	push		eax
	popfd
	pushfd
	pop			eax
	cmp			eax , ebx
	jz			NO_CPUID

	// function 00h - query standard features
	xor			eax , eax
	cpuid
	
	mov			DWORD PTR [edi][_processor_info::v_name][0]  , ebx
	mov			DWORD PTR [edi][_processor_info::v_name][4]  , edx
	mov			DWORD PTR [edi][_processor_info::v_name][8]  , ecx
	mov			BYTE PTR  [edi][_processor_info::v_name][12] , 0

	// check for greater function presence
	test		eax , eax
	jz			CHECK_EXT

	// function 01h - feature sets
	mov			eax , 01h
	cpuid

	// stepping ID
	mov			ebx , eax
	and			ebx , 0fh
	mov			BYTE PTR [edi][_processor_info::stepping] , bl

	// Model
	mov			ebx , eax
	shr			ebx , 04h
	and			ebx , 0fh
	mov			BYTE PTR [edi][_processor_info::model] , bl

	// Family
	mov			ebx , eax
	shr			ebx , 08h
	and			ebx , 0fh
	mov			BYTE PTR [edi][_processor_info::family] , bl

	// Raw features
	// TODO: check against vendor

	// Against SSE3
	xor			ebx , ebx
	mov			eax , _CPU_FEATURE_SSE3
	test		ecx , 01h
	cmovnz		ebx , eax
	or			esi , ebx

	// Against SSSE3
	xor			ebx , ebx
	mov			eax , _CPU_FEATURE_SSSE3
	test		ecx , 0200h
	cmovnz		ebx , eax
	or			esi , ebx

	// Against SSE4.1
	xor			ebx , ebx
	mov			eax , _CPU_FEATURE_SSE4_1
	test		ecx , 080000h
	cmovnz		ebx , eax
	or			esi , ebx

	// Against SSE4.2
	xor			ebx , ebx
	mov			eax , _CPU_FEATURE_SSE4_2
	test		ecx , 0100000h
	cmovnz		ebx , eax
	or			esi , ebx

	// Against MMX
	xor			ebx , ebx
	mov			eax , _CPU_FEATURE_MMX
	test		edx , 0800000h
	cmovnz		ebx , eax
	or			esi , ebx

	// Against SSE
	xor			ebx , ebx
	mov			eax , _CPU_FEATURE_SSE
	test		edx , 02000000h
	cmovnz		ebx , eax
	or			esi , ebx

	// Against SSE2
	xor			ebx , ebx
	mov			eax , _CPU_FEATURE_SSE2
	test		edx , 04000000h
	cmovnz		ebx , eax
	or			esi , ebx

CHECK_EXT:
	// test for extended functions
	mov			eax , 80000000h
	cpuid
	cmp			eax , 80000004h
	jb			NO_CPUID

	// first 16 bytes
	mov			eax , 80000002h
	cpuid

	mov			DWORD PTR [edi][_processor_info::model_name][0]  , eax
	mov			DWORD PTR [edi][_processor_info::model_name][4]  , ebx
	mov			DWORD PTR [edi][_processor_info::model_name][8]  , ecx
	mov			DWORD PTR [edi][_processor_info::model_name][12] , edx

	// second 16 bytes
	mov			eax , 80000003h
	cpuid

	mov			DWORD PTR [edi][_processor_info::model_name][16] , eax
	mov			DWORD PTR [edi][_processor_info::model_name][20] , ebx
	mov			DWORD PTR [edi][_processor_info::model_name][24] , ecx
	mov			DWORD PTR [edi][_processor_info::model_name][28] , edx

	// third 16 bytes
	mov			eax , 80000004h
	cpuid

	mov			DWORD PTR [edi][_processor_info::model_name][32] , eax
	mov			DWORD PTR [edi][_processor_info::model_name][36] , ebx
	mov			DWORD PTR [edi][_processor_info::model_name][40] , ecx
	mov			DWORD PTR [edi][_processor_info::model_name][44] , edx

	// trailing zero
	mov			BYTE PTR  [edi][_processor_info::model_name][48] , 0	

	// trimming initials
	mov			ax , 020h

	// trailing spaces
	xor			ebx , ebx

TS_FIND_LOOP:
	cmp			BYTE PTR  [edi][ebx][_processor_info::model_name] , ah
	jz			TS_FIND_EXIT
	inc			ebx
	jmp short	TS_FIND_LOOP
TS_FIND_EXIT:

	test		ebx , ebx
	jz			TS_MOVE_EXIT

TS_MOVE_LOOP:
	dec			ebx
	cmp			BYTE PTR  [edi][ebx][_processor_info::model_name] , al
	jnz			TS_MOVE_EXIT
	mov			BYTE PTR  [edi][ebx][_processor_info::model_name] , ah
	jmp short	TS_MOVE_LOOP

TS_MOVE_EXIT:

	// heading spaces
	xor			ebx , ebx

HS_FIND_LOOP:
	cmp			BYTE PTR  [edi][ebx][_processor_info::model_name] , al
	jnz			HS_FIND_EXIT
	inc			ebx
	jmp short	HS_FIND_LOOP
HS_FIND_EXIT:

	test		ebx , ebx
	jz			HS_MOVE_EXIT

	xor			edx , edx

HS_MOVE_LOOP:
	mov			cl , BYTE PTR  [edi][ebx][_processor_info::model_name]
	mov			BYTE PTR  [edi][edx][_processor_info::model_name] , cl
	inc			ebx
	inc			edx
	test		cl , cl
	jnz			HS_MOVE_LOOP

HS_MOVE_EXIT:	

	// many spaces
	xor			ebx , ebx

MS_FIND_LOOP:
	// 1st character
	mov			cl , BYTE PTR  [edi][ebx][_processor_info::model_name]
	test		cl , cl
	jz			MS_FIND_EXIT
	cmp			cl , al
	jnz			MS_FIND_NEXT
	// 2nd character
	mov			edx , ebx
	inc			ebx
	mov			cl , BYTE PTR  [edi][ebx][_processor_info::model_name]
	test		cl , cl
	jz			MS_FIND_EXIT
	cmp			cl , al
	jnz			MS_FIND_NEXT
	// move
	jmp short	HS_MOVE_LOOP

MS_FIND_NEXT:
	inc			ebx
	jmp short	MS_FIND_LOOP
MS_FIND_EXIT:

NO_CPUID:
	
	mov		DWORD PTR [edi][_processor_info::feature] , esi
}

	return pinfo->feature;
}

#else

// These are the bit flags that get set on calling cpuid
// with register eax set to 1
#define _MMX_FEATURE_BIT			0x00800000
#define _SSE_FEATURE_BIT			0x02000000
#define _SSE2_FEATURE_BIT			0x04000000

// This bit is set when cpuid is called with
// register set to 80000001h (only applicable to AMD)
#define _3DNOW_FEATURE_BIT			0x80000000
 
int IsCPUID()
{
    __try {
        _asm
        {
            xor eax, eax
            cpuid
        }
    } __except ( EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
    return 1;
}

// borland doesn't understand MMX/3DNow!/SSE/SSE2 asm opcodes
void _os_support(int feature, int& res)
{
	res |= feature;
}

/***
*
* void map_mname(int, int, const char *, char *) maps family and model to processor name
*
****************************************************/

void map_mname( int family, int model, const char * v_name, char *m_name)
{
    if (!strncmp("AuthenticAMD", v_name, 12))
    {
        switch (family) // extract family code
        {
        case 4: // Am486/AM5x86
            strcpy (m_name,"Am486");
            break;

        case 5: // K6
            switch (model) // extract model code
            {
            case 0:		strcpy (m_name,"K5 Model 0");	break;
            case 1:		strcpy (m_name,"K5 Model 1");	break;
            case 2:		strcpy (m_name,"K5 Model 2");	break;
            case 3:		strcpy (m_name,"K5 Model 3");	break;
            case 4:     break;	// Not really used
            case 5:     break;  // Not really used
            case 6:		strcpy (m_name,"K6 Model 1");	break;
            case 7:		strcpy (m_name,"K6 Model 2");	break;
            case 8:		strcpy (m_name,"K6-2");			break;
            case 9: 
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:	strcpy (m_name,"K6-3");			break;
            default:	strcpy (m_name,"K6 family");	break;
            }
            break;

        case 6: // Athlon
            switch(model)  // No model numbers are currently defined
            {
            case 1:		strcpy (m_name,"ATHLON Model 1");	break;
			case 2:		strcpy (m_name,"ATHLON Model 2");	break;
			case 3:		strcpy (m_name,"DURON");			break;
			case 4:	
			case 5:		strcpy (m_name,"ATHLON TB");		break;
			case 6:		strcpy (m_name,"ATHLON XP");		break;
			case 7:		strcpy (m_name,"DURON XP");			break;
            default:    strcpy (m_name,"K7 Family");		break;
			}
            break;
        }
    } else if ( !strncmp("GenuineIntel", v_name, 12))
    {
        switch (family) // extract family code
        {
        case 4:
            switch (model) // extract model code
            {
            case 0:
            case 1:		strcpy (m_name,"i486DX");			break;
            case 2:		strcpy (m_name,"i486SX");			break;
            case 3:		strcpy (m_name,"i486DX2");			break;
            case 4:		strcpy (m_name,"i486SL");			break;
            case 5:		strcpy (m_name,"i486SX2");			break;
            case 7:		strcpy (m_name,"i486DX2E");			break;
            case 8:		strcpy (m_name,"i486DX4");			break;
            default:    strcpy (m_name,"i486 family");		break;
            }
            break;
        case 5:
            switch (model) // extract model code
            {
            case 1:
            case 2:
            case 3:		strcpy (m_name,"Pentium");			break;
            case 4:		strcpy (m_name,"Pentium-MMX");		break;
            default:	strcpy (m_name,"P5 family");		break;
            }
            break;
        case 6:
            switch (model) // extract model code
            {
            case 1:		strcpy (m_name,"Pentium-Pro");		break;
            case 3:		strcpy (m_name,"Pentium-II");		break;
            case 5:		strcpy (m_name,"Pentium-II");		break;  // actual differentiation depends on cache settings
            case 6:		strcpy (m_name,"Celeron");			break;
            case 7:		strcpy (m_name,"Pentium-III");		break;  // actual differentiation depends on cache settings
			case 8:		strcpy (m_name,"P3 Coppermine");	break;
            default:	strcpy (m_name,"P3 family");		break;
            }
            break;
		case 15:
			// F15/M2/S4 ???
			switch (model)
			{
			case 2:		strcpy	(m_name,"Pentium 4");		break;
			default:	strcpy	(m_name,"P4 family");		break;
			}
        }
    } else if ( !strncmp("CyrixInstead", v_name,12))
    {
        strcpy (m_name,"Unknown");
    } else if ( !strncmp("CentaurHauls", v_name,12))
    {
        strcpy (m_name,"Unknown");
    } else 
    {
        strcpy (m_name, "Unknown");
    }

}

/***
*
* int _cpuid (_p_info *pinfo)
* 
* Entry:
*
*   pinfo: pointer to _p_info.
*
* Exit:
*
*   Returns int with capablity bit set even if pinfo = NULL
*
****************************************************/


int _cpuid (_processor_info *pinfo)
{
    u32 dwStandard = 0;
    u32 dwFeature = 0;
    u32 dwMax = 0;
    u32 dwExt = 0;
    int feature = 0, os_support = 0;
    union
    {
        char cBuf[12+1];
        struct
        {
            u32 dw0;
            u32 dw1;
            u32 dw2;
        };
    } Ident;

    if (!IsCPUID())
    {
        return 0;
    }

    _asm
    {
        push ebx
        push ecx
        push edx

        // get the vendor string
        xor eax,eax
        cpuid
        mov dwMax,eax
        mov dword ptr Ident.dw0,ebx
        mov dword ptr Ident.dw1,edx
        mov dword ptr Ident.dw2,ecx

        // get the Standard bits
        mov eax,1
        cpuid
        mov dwStandard,eax
        mov dwFeature,edx

        // get AMD-specials
        mov eax,80000000h
        cpuid
        cmp eax,80000000h
        jc notamd
        mov eax,80000001h
        cpuid
        mov dwExt,edx

notamd:
        pop ecx
        pop ebx
        pop edx
    }

    if (dwFeature & _MMX_FEATURE_BIT)
    {
        feature |= _CPU_FEATURE_MMX;
        _os_support(_CPU_FEATURE_MMX,os_support);
    }
    if (dwExt & _3DNOW_FEATURE_BIT)
    {
        feature |= _CPU_FEATURE_3DNOW;
        _os_support(_CPU_FEATURE_3DNOW,os_support);
    }
    if (dwFeature & _SSE_FEATURE_BIT)
    {
        feature |= _CPU_FEATURE_SSE;
        _os_support(_CPU_FEATURE_SSE,os_support);
    }
    if (dwFeature & _SSE2_FEATURE_BIT)
    {
        feature |= _CPU_FEATURE_SSE2;
        _os_support(_CPU_FEATURE_SSE2,os_support);
    }

	if (pinfo)
    {
        memset		(pinfo, 0, sizeof(_processor_info));
        pinfo->os_support = os_support;
        pinfo->feature = feature;
        pinfo->family = (dwStandard >> 8)&0xF;  // retriving family
        pinfo->model = (dwStandard >> 4)&0xF;   // retriving model
        pinfo->stepping = (dwStandard) & 0xF;   // retriving stepping
        Ident.cBuf[12] = 0;
        strcpy_s		(pinfo->v_name, Ident.cBuf);
        map_mname	(pinfo->family, pinfo->model, pinfo->v_name, pinfo->model_name);
    }
   return feature;
}
#endif
