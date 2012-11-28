#pragma once
#include "object_interfaces.h"
#include "script_export_space.h"
//#include <luabind/object.hpp>

enum TypeOfData {
	lua_nil,
	lua_bool,
	lua_string,
	lua_number,
	lua_vector,
	lua_table
};

struct StoreData {
	void* data;
	TypeOfData type;
};

class CStoreHouse
{
public:
	CStoreHouse(void);
	virtual ~CStoreHouse(void);

	void			add_boolean		(LPCSTR name, bool b);
	void			add_float		(LPCSTR name, float number);
	void			add_string		(LPCSTR name, LPCSTR string);
	void			add_vector		(LPCSTR name, Fvector v);
	void			add_number		(LPCSTR name, double number);
	void			add_table		(LPCSTR name, LPCSTR string);

	bool			get_boolean		(LPCSTR name);
	LPCSTR			get_string		(LPCSTR name);
	double			get_number		(LPCSTR name);
	Fvector			get_vector		(LPCSTR name);
	LPCSTR			get_table		(LPCSTR name);



	LPCSTR			get_data_type	(LPCSTR name);
	bool			data_exist		(LPCSTR name);
	void			delete_data		(LPCSTR name);


	void save(IWriter &memory_stream);
	void load(IReader &file_stream);

private:
	void add(shared_str name, void* ptr_data, size_t size, TypeOfData _type);
	void get(shared_str name, void* ptr,u32 size);
	void add_data_exist(shared_str name);
	void get_data_exist(shared_str name);
	bool update(shared_str name, void* ptr_data, size_t size, TypeOfData _type);
	u32 type_to_size(StoreData d);

	//size_t m_size;//m_size is not need. it equal data.size()

	xr_map<shared_str,StoreData> data;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};


add_to_type_list(CStoreHouse)
#undef script_type_list
#define script_type_list save_type_list(CStoreHouse)