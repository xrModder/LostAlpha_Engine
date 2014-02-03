#include "stdafx.h"

#include <dxsdk\Include\D3D10_1.h>
#include <dxsdk\Include\D3Dx10core.h>
#include <dxsdk\Include\D3DCompiler.h>

#include "BC.h"

#include <directxmath.h>
#include <directxpackedvector.h>

typedef void DUMMY_STUFF (const void*,const u32&,void*);
XRCORE_API DUMMY_STUFF	*g_temporary_stuff;
XRCORE_API DUMMY_STUFF	*g_dummy_stuff;

#define PROTECTED_BUILD

#ifdef PROTECTED_BUILD
#	define TRIVIAL_ENCRYPTOR_ENCODER
#	define TRIVIAL_ENCRYPTOR_DECODER
#	include "../xrCore/trivial_encryptor.h"
#	undef TRIVIAL_ENCRYPTOR_ENCODER
#	undef TRIVIAL_ENCRYPTOR_DECODER
#endif // PROTECTED_BUILD

DEFINE_VECTOR(char*, file_list, file_list_it);
typedef xr_vector<char*> file_list;

string_path g_folder_path = { 0 };

ID3D10Device* g_device = 0;

void __cdecl clMsg(const char *format, ...)
{
	va_list		mark;
	string1024	buff;
	va_start	(mark, format);
	vsprintf	(buff, format, mark);
	printf		(buff);
	printf		("\n");
	Log			(buff);
}

/*

basicly the idea of this:
if lmap#*_2.dds has alpha channel, need to switch alpha and rgb in photoshop.
if map#*_2.dds hasn't alpha channel, need to add black alpha channel to lmap#*_1.dds, and in lmap#*_2.dds rgb channel move to alpha, and flood rgb with black.. in photoshop too.

*/

void usage()
{
	static LPCSTR help = 
		"The following keys are supported / required:\n"
		"-? or -h		== this help\n"
		"-f <NAME>		== level in gamedata\\levels\\<NAME>\\\n";
	clMsg(help);
}

void parse_params(LPCSTR params)
{
	string512 cmd, temp;
	xr_strcpy(cmd, params);
	strlwr(cmd);
	if (strstr(cmd, "-?") || strstr(cmd, "-h") || !strstr(cmd, "-f"))	
	{
		usage();
		TerminateProcess(GetCurrentProcess(), 0);
	}
	sscanf(strstr(cmd,"-f") + 2, "%s", temp);	
	FS.update_path(g_folder_path, "$game_levels$", temp);
	xr_strcat(g_folder_path, "\\");
	clMsg("level selected: %s", temp);
}

void create_device()
{
	// fast creation, no swap chain
	R_CHK(D3D10CreateDevice(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG, D3D10_SDK_VERSION, &g_device));	
}

void destroy_device()
{
	g_device->Release();
	g_device = 0;
}

ID3D10Texture2D* load_dds(LPCSTR file_name)
{
	IReader* reader = NULL;
	ID3D10Resource *texture;
	D3DX10_IMAGE_LOAD_INFO loader;
	D3DX10_IMAGE_INFO info;

	ZeroMemory(&info, sizeof(D3DX10_IMAGE_INFO));
	reader = FS.r_open(file_name);

	R_CHK(D3DX10GetImageInfoFromMemory(reader->pointer(), reader->length(), 0, &info, 0));
	clMsg("loaded %s: %d", file_name, info.Format); //it's DXGI_FORMAT_BC3_UNORM -> D3DFMT_DXT4

	loader.Usage = D3D10_USAGE_STAGING;
	loader.Width = info.Width;
	loader.Height = info.Height;
	loader.BindFlags = 0; // very important for reading!
	loader.CpuAccessFlags = D3D10_CPU_ACCESS_READ;
	loader.pSrcInfo = &info;
	loader.Format = info.Format;
	loader.MipLevels = info.MipLevels;

	R_CHK(D3DX10CreateTextureFromMemory(g_device, reader->pointer(), reader->length(), &loader, 0, &texture, 0));

	FS.r_close(reader);
	
	return (ID3D10Texture2D*) texture;
}

ID3D10Texture2D* reload_dds_for_writing(ID3D10Texture2D* dds)
{
	ID3D10Texture2D* new_dds = 0;
	D3DX10_TEXTURE_LOAD_INFO loader;
	D3D10_TEXTURE2D_DESC new_dds_descr;
	D3D10_TEXTURE2D_DESC dds_descr;

	dds->GetDesc(&dds_descr);

	ZeroMemory(&new_dds_descr, sizeof(D3D10_TEXTURE2D_DESC));
	new_dds_descr.Format = dds_descr.Format;
	new_dds_descr.BindFlags = 0;
	new_dds_descr.MiscFlags = 0;
	new_dds_descr.Width = dds_descr.Width;
	new_dds_descr.Height = dds_descr.Height;
	new_dds_descr.MipLevels = dds_descr.MipLevels; 
	new_dds_descr.ArraySize = 1;
	new_dds_descr.SampleDesc.Count = 1;
	new_dds_descr.SampleDesc.Quality = 0;
	new_dds_descr.Usage = D3D10_USAGE_STAGING;
	new_dds_descr.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
	R_CHK(g_device->CreateTexture2D(&new_dds_descr, NULL, &new_dds));

	D3D10_BOX source_box;
	D3D10_BOX dest_box;
	ZeroMemory(&source_box, sizeof(D3D10_BOX));
	ZeroMemory(&dest_box, sizeof(D3D10_BOX));
	source_box.left = dest_box.left = 0;
	source_box.top = dest_box.top = 0;
	source_box.front = dest_box.front = 0;
	source_box.right = dds_descr.Width;
	source_box.bottom = dds_descr.Height;
	dest_box.bottom = dds_descr.Height;
	dest_box.right = dds_descr.Width;
	source_box.back = dest_box.back = 0;

	ZeroMemory(&loader, sizeof(D3DX10_TEXTURE_LOAD_INFO));
	loader.pSrcBox = &source_box;
	loader.pDstBox = &dest_box;
	loader.SrcFirstMip = D3D10CalcSubresource(0, 0, 1);
	loader.DstFirstMip = D3D10CalcSubresource(0, 0, 1);
	loader.NumMips = 1;
	loader.SrcFirstElement = 0;
	loader.DstFirstElement = 0;
	loader.NumElements = 0;
	loader.Filter = D3DX10_FILTER_LINEAR;
	loader.MipFilter = D3DX10_FILTER_LINEAR;

	HRESULT result = D3DX10LoadTextureFromTexture(dds, &loader, new_dds);
	LOG_IF_FAIL_DX(result);
	R_CHK(result);
	return new_dds;
}

void save_dds(LPCSTR file_name, ID3D10Texture2D *dds)
{
	IWriter* writer = NULL;
	string_path path;
	xr_sprintf(path, "%s.old", file_name);
	FS.file_rename(file_name, path);
	R_CHK(D3DX10SaveTextureToFile(dds, D3DX10_IFF_DDS, file_name));
}

char* rstrstr(LPCSTR s1, LPCSTR s2)
{
	size_t len1 = strlen(s1);
	size_t len2 = strlen(s2);
	const char* s = NULL;
	if (len2 > len1)
		return NULL;
	for (s = s1 + len1 - len2; s >= s1; --s)
		if (strncmp(s, s2, len2) == 0)
			break;
	return const_cast<char*>(s);
}

void process_lmap(LPCSTR file_name)
{
	u32 lmap_id = atoi(rstrstr(file_name, "_") + 1);
	clMsg("Processing: %s, %d", file_name, lmap_id);
	if (lmap_id == 1) 
		return;
	D3D10_TEXTURE2D_DESC wdescr;
	D3D10_MAPPED_TEXTURE2D mapped_wdds;
	ID3D10Texture2D* dds = load_dds(file_name);
	ID3D10Texture2D* wdds = reload_dds_for_writing(dds);
	wdds->GetDesc(&wdescr);
	//R_CHK(dds->Map(D3D10CalcSubresource(0, 0, 1), D3D10_MAP_READ, 0, &mapped_dds)); // subres: 0
	R_CHK(wdds->Map(D3D10CalcSubresource(0, 0, 1), D3D10_MAP_READ_WRITE, 0, &mapped_wdds));
	u8* src = (u8*) mapped_wdds.pData;
	switch (wdescr.Format)
	{
		case DXGI_FORMAT_BC3_UNORM: // DXT4
		{
			for (u32 row = 0; row < wdescr.Height; row += 4)
			{
				u8* ptr = src;
				for (u32 col = 0; col < mapped_wdds.RowPitch; col += 16)
				{
					try
					{
						using namespace DirectX; 
						XMVECTOR temp[16];				
						D3DXDecodeBC3(temp, ptr);
						for (u8 i = 0; i < 16; i++)
						{
							XMFLOAT4 clr, new_clr;
							_mm_storeu_ps(&(clr.x), temp[i]);
							new_clr.x = clr.w;
							new_clr.y = clr.w;
							new_clr.z = clr.w;
							new_clr.w = clr.x; // (clr.x + clr.y + clr.z + clr.w) / 4;
							temp[i] = _mm_loadu_ps(&(new_clr.x));
						}
						D3DXEncodeBC3(ptr, temp, BC_FLAGS_NONE);
						ptr += 16;
					}
					catch (...)
					{
						clMsg("error: %dx%d", row, col);
						return;
					}
				}
				src += mapped_wdds.RowPitch;
			}
			break;
		}
		default:
		{
			clMsg("error unattended format: %d", wdescr.Format);
			break;
		}
	}	
	//dds->Unmap(D3D10CalcSubresource(0, 0, 1));
	wdds->Unmap(D3D10CalcSubresource(0, 0, 1));
	save_dds(file_name, wdds);
	dds->Release();
	wdds->Release();
}

void execute()
{
	const CLocatorAPI::file* folder = FS.exist(g_folder_path);
	R_ASSERT(folder);
	file_list* list = FS.file_list_open(folder->name, FS_ListFiles);
	R_ASSERT(list && !list->empty());
	create_device();
	for (file_list_it it = list->begin(), last = list->end(); it != last; ++it)
	{
		string256 name, ext;
		_splitpath(*it, 0, 0, name, ext);
		if (!stricmp(ext, ".dds") && strstr(*it, "lmap"))
		{
			string_path file;
			xr_sprintf(file, "%s%s", g_folder_path, *it);
			process_lmap(file);
		}

	}
	FS.file_list_close(list);
	destroy_device();
}


int __cdecl main	(int argc, char* argv[])
{
	g_temporary_stuff	= &trivial_encryptor::decode;
	g_dummy_stuff		= &trivial_encryptor::encode;
	Debug._initialize	(false);
	Core._initialize	("xrlmaps_la", NULL, TRUE, NULL);
	
	parse_params		(GetCommandLine());
	execute				();

	Core._destroy		();
	return 0;
}