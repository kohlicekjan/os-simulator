#pragma once

#include "..\api\api.h"

namespace kiv_os_rtl {


	size_t Get_Last_Error();

	kiv_os::THandle Create_File(const char* file_name, size_t flags, size_t isDir = 0);
	//podle flags otevre, vytvori soubor a vrati jeho deskriptor
	//vraci nenulovy handle, kdyz vse OK
	bool Write_File(const kiv_os::THandle file_handle, const void *buffer, const size_t buffer_size, size_t &written);
	//zapise do souboru identifikovaneho deskriptor data z buffer o velikosti buffer_size a vrati pocet zapsanych dat ve written
	//vraci true, kdyz vse OK

	//cte ze souboru data o urcite velikosti
	bool Read_File(const kiv_os::THandle file_handle, const void *buffer, const size_t buffer_size, size_t *filled);

	bool Delete_File(const char* file_name, size_t isDir);

	bool Set_File_Position(const kiv_os::THandle file_handle, const size_t new_position);

	bool Get_File_Position(const kiv_os::THandle file_handle, const uint8_t position_type);

	bool Close_File(const kiv_os::THandle file_handle);
	//uzavre soubor identifikovany pomoci deskriptoru
	//vraci true, kdyz vse OK

	bool Get_Current_Directory(const char * buffer, const size_t buffer_size);

	bool Set_Current_Directory(const char * buffer);

	bool Create_Pipe(kiv_os::THandle file_in, kiv_os::THandle file_out, int count, kiv_os::THandle pipe[2]);

	bool Create_Process(kiv_os::TRegisters &regs);

	bool Wait_For(kiv_os::TRegisters &regs);

}