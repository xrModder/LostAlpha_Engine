#include "stdafx.h"
#include "Store.h"
#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>

CStoreHouse::CStoreHouse(void) 
{
}

CStoreHouse::~CStoreHouse(void) 
{
	while (data.size())
	{
		xr_delete(data.begin()->second.data);
		data.erase(data.begin());
	}
	Memory.mem_compact();
}

void CStoreHouse::add(shared_str name, void* ptr_data, size_t size, TypeOfData _type)
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

bool CStoreHouse::update(shared_str name, void *ptr_data, size_t size, TypeOfData _type)
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
	R_ASSERT2(0,"StoreHouse unknown type");
	return u32(-1);
}



void CStoreHouse::save(IWriter &memory_stream)
{
	xr_map<shared_str,StoreData>::iterator it, last;

	memory_stream.w_u64(data.size());
	for (it=data.begin(),last=data.end();it!=last;++it)
	{
		memory_stream.w_stringZ(it->first);
		memory_stream.w_u8(it->second.type);
		memory_stream.w(it->second.data, type_to_size(it->second));
	}
}

void CStoreHouse::load(IReader &file_stream)
{
	//maybe you think m_size member value, not local :)
	size_t m_size = file_stream.r_u64();
	for (size_t i=0;i<m_size;i++) {
		StoreData d;
		shared_str name;
		file_stream.r_stringZ(name);
		d.type = (TypeOfData)file_stream.r_u8();
		if (d.type==lua_string) {
			shared_str s_data;
			file_stream.r_stringZ(s_data);
			
			u32 size = sizeof(char)*(s_data.size()+1);
			void* ptr = xr_malloc(size);
			xr_memcpy(ptr,s_data.c_str(),size);

			d.data = ptr;
		} else {
			void* ptr = xr_malloc(type_to_size(d));
			//CopyMemory(ptr,ptr_data,size);
			file_stream.r(ptr, type_to_size(d));
			d.data = ptr;
		}
		data[name.c_str()]=d;
	}
}
#pragma warning(pop)