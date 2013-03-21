#include "stdafx.h"
#include "Store.h"
#include "pch_script.h"
#include "alife_space.h"
#include "script_engine.h"
#include "ai_space.h"
#include "alife_simulator.h"


CStoreHouse::~CStoreHouse() 
{
	while (data.size())
	{
		xr_delete(data.begin()->second.data);
		data.erase(data.begin());
	}
	data.clear();
	Memory.mem_compact();
}

void CStoreHouse::add(shared_str name, void* ptr_data, u32 size, TypeOfData _type)
{
	//add_data_exist(name);
	if (update(name, ptr_data, size, _type)) return;

	void* ptr = xr_malloc(size);
	xr_memcpy(ptr,ptr_data,size);

	StoreData d;
	d.data = ptr;
	d.type = _type;
	data[name] = d;
}

void CStoreHouse::add_boolean(LPCSTR name,bool b)
{
	add(name,&b,sizeof(b),lua_bool);
}

void CStoreHouse::add_number(LPCSTR name, double number)
{
	add(name,&number,sizeof(number),lua_number);
}

void CStoreHouse::add_string(LPCSTR name, LPCSTR string)
{
	add(name,(void*)string,xr_strlen(string)+1,lua_string);
}

void CStoreHouse::add_vector(LPCSTR name, Fvector v)
{
	add(name,&v,sizeof(float)*3,lua_vector);
}

void CStoreHouse::add_table(LPCSTR name, LPCSTR string)
{
	add(name,(void*)string,xr_strlen(string)+1,lua_table);
}

bool CStoreHouse::update(shared_str name, void *ptr_data, u32 size, TypeOfData _type)
{
	xr_map<shared_str,StoreData>::iterator it = data.find(name);
	if (it == data.end()) 
		return false;
//	it->second.type = _type;
	xr_delete(it->second.data);
	it->second.data = xr_malloc(size);
	xr_memcpy(it->second.data,ptr_data,size);
	return true;
}

void CStoreHouse::add_data_exist(shared_str name)
{
	xr_map<shared_str,StoreData>::iterator it = data.find(name);
	R_ASSERT3(data.find(name)==data.end(),"Can't save data with the same name ",name.c_str());
}

void CStoreHouse::get_data_exist(shared_str name)
{
	xr_map<shared_str,StoreData>::iterator it = data.find(name);
	xr_map<shared_str,StoreData>::iterator it_end = data.end();

	R_ASSERT3(data.find(name)!=data.end(),"Data doesn't exist! ",name.c_str());
}

void CStoreHouse::delete_data(LPCSTR c_name)
{
	shared_str name(c_name);
	xr_map<shared_str,StoreData>::iterator it = data.find(name);
	R_ASSERT3(it!=data.end(),"Data doesn't exist! ",name.c_str());
	xr_delete(it->second.data);
	data.erase(it);
}

void CStoreHouse::get(shared_str name,void* p,u32 size)
{
	get_data_exist(name);
	CopyMemory(p,data[name].data,size);
}

bool CStoreHouse::get_boolean(LPCSTR name)
{
	bool tmp;
	get(name,&tmp,sizeof(bool));
	return tmp;
}

double CStoreHouse::get_number(LPCSTR name)
{
	double tmp;
	get(name,&tmp,sizeof(tmp));
	return tmp;
}

LPCSTR CStoreHouse::get_string(LPCSTR name)
{
	shared_str s;
	get_data_exist(name);
	return LPCSTR(data[name].data);
}

Fvector CStoreHouse::get_vector(LPCSTR name)
{
	Fvector v;
	get(name,&v,sizeof(float)*3);
	return v;
}

LPCSTR CStoreHouse::get_table(LPCSTR name)
{
	shared_str s;
	get_data_exist(name);
	return LPCSTR(data[name].data);
}

bool CStoreHouse::data_exist(LPCSTR name)
{
	shared_str str(name);
	xr_map<shared_str, StoreData>::iterator it= data.find(str);
	xr_map<shared_str, StoreData>::iterator it_end = data.end();
	bool result = it!=it_end;
	return result;
}

LPCSTR CStoreHouse::get_data_type(LPCSTR name)
{
	shared_str s_name(name);
	//if (data_exist(name)) {
	xr_map<shared_str, StoreData>::iterator it = data.find(s_name);
	if (it!=data.end()){
		switch(it->second.type) {
			case lua_bool : return "boolean";
			case lua_vector : return "vector";
			case lua_string : return "string";
			case lua_nil : return "nil";
			case lua_number : return "number";
			case lua_table : return "table";
		}
	}
	return "ERROR";
}

u32 CStoreHouse::type_to_size(StoreData d)
{
	switch (d.type) {
		case lua_bool: return sizeof(bool);
		case lua_table:
		case lua_string: return xr_strlen((char*)d.data) * sizeof(char)+1;
		case lua_vector: return 3*sizeof(float);
		case lua_number: return sizeof(double);
	};
	R_ASSERT2(0,make_string("StoreHouse unknown type [%d]", d.type));
	return u32(-1);
}



void CStoreHouse::save(IWriter &memory_stream)
{
	Msg("* Writing Store...");
	memory_stream.open_chunk	(STORE_CHUNK_DATA);
	xr_map<shared_str,StoreData>::iterator it, last;

	memory_stream.w_u64(data.size());
	for (it=data.begin(),last=data.end();it!=last;++it)
	{
		memory_stream.w_stringZ(it->first);
		memory_stream.w_u8(it->second.type);
		memory_stream.w(it->second.data, type_to_size(it->second));
	}
	memory_stream.close_chunk	();

	Msg("* %d store values successfully saved", data.size());
}

void CStoreHouse::load(IReader &file_stream)
{
	R_ASSERT2					(file_stream.find_chunk(STORE_CHUNK_DATA),"Can't find chunk STORE_CHUNK_DATA!");
	Msg("* Loading Store...");
	size_t count = file_stream.r_u64();
	for (size_t i=0;i<count;i++) {
		StoreData d;
		shared_str name;
		file_stream.r_stringZ(name);
		d.type = (TypeOfData)file_stream.r_u8();
		switch (d.type)
		{
			case lua_string:
			case lua_table:
			{
				shared_str s_data;
				file_stream.r_stringZ(s_data);
				
				u32 size = sizeof(char)*(s_data.size()+1);
				void* ptr = xr_malloc(size);
				xr_memcpy(ptr,s_data.c_str(),size);

				d.data = ptr;
				break;
			}
			default:
			{
				u32 size = type_to_size(d);
				void* ptr = xr_malloc(size);
				file_stream.r(ptr, size);
				d.data = ptr;
				break;
			}
		}
		data[name.c_str()]=d;

		//script callback
		bool loaded = ((u8)i==(u8)count)?true:false;

		if (pSettings->section_exist("lost_alpha_cfg") && pSettings->line_exist("lost_alpha_cfg","on_load_store_callback"))
		{
			string256					func_name;
			luabind::functor<void>			func;
			strcpy_s(func_name,pSettings->r_string("lost_alpha_cfg","on_load_store_callback"));
			R_ASSERT					(ai().script_engine().functor<void>(func_name,func));

			switch (d.type)
			{
				case lua_bool : func(name.c_str(), get_boolean(name.c_str()),loaded); break;
				case lua_vector : func(name.c_str(), get_vector(name.c_str()),loaded); break;
				case lua_string : func(name.c_str(), get_string(name.c_str()),loaded); break;
				case lua_number : func(name.c_str(), get_number(name.c_str()),loaded); break;
				case lua_table : func(name.c_str(), get_table(name.c_str()),loaded); break;
			}
		}
	}
	Msg("* %d store values successfully loaded", count);

}