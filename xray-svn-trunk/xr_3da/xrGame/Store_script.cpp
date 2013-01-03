#include "stdafx.h"
#include "Store.h"
#include "pch_script.h"

using namespace luabind;

#pragma optimize("s",on)


//void script_call_add_u64(CStoreHouse* self,LPCSTR name)//,unsigned	__int64 value)
//{
//	self->add_u64(0);
//}

void CStoreHouse::script_register(lua_State *L)
{
	module(L)
		[

			class_<CStoreHouse>("StoreHouse")
				.def(constructor<>())
				//.def("add_bool",					&CStoreHouse::add_bool)
				//.def("add_u8",						&CStoreHouse::add_u8)
				//.def("add_s8",						&CStoreHouse::add_s8)
				//.def("add_u16",						&CStoreHouse::add_u16)
				//.def("add_s16",						&CStoreHouse::add_s16)
				//.def("add_u32",						&CStoreHouse::add_u32)
				//.def("add_s32",						&CStoreHouse::add_s32)
				//.def("add_u64",						&script_call_add_u64)//&CStoreHouse::add_u64)
				//.def("add_s64",						&CStoreHouse::add_s64)
				.def("add_string",					&CStoreHouse::add_string)
				//.def("add_float",					&CStoreHouse::add_float)
				.def("add_vector",					&CStoreHouse::add_vector)
				.def("add_boolean",					&CStoreHouse::add_boolean)
				.def("add_number",					&CStoreHouse::add_number)
				.def("add_table",					&CStoreHouse::add_table)


				//.def("get_u8",						&CStoreHouse::get_u8)
				//.def("get_s8",						&CStoreHouse::get_s8)
				//.def("get_u16",						&CStoreHouse::get_u16)
				//.def("get_s16",						&CStoreHouse::get_s16)
				//.def("get_u32",						&CStoreHouse::get_u32)
				//.def("get_s32",						&CStoreHouse::get_s32)
//				.def("get_u64",						&CStoreHouse::get_u64)
//				.def("get_s64",						&CStoreHouse::get_s64)
				.def("get_string",					&CStoreHouse::get_string)
				//.def("get_float",					&CStoreHouse::get_float)
				.def("get_vector",					&CStoreHouse::get_vector)
				.def("get_number",					&CStoreHouse::get_number)
				.def("get_boolean",					&CStoreHouse::get_boolean)
				.def("get_table",					&CStoreHouse::get_table)

				.def("get_data_type",				&CStoreHouse::get_data_type)
				.def("data_exist",					&CStoreHouse::data_exist)
				.def("get_vector",					&CStoreHouse::get_vector)
				
				.def("delete_data",					&CStoreHouse::delete_data)
		];
}


