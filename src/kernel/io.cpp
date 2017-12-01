#pragma once

#include "io.h"
#include "kernel.h"
#include "handles.h"
#include "stdio.h"
#include "file_descriptor.h"
#include "file_system.h"
#include "process.h"


void HandleIO(kiv_os::TRegisters &regs) {

	//V ostre verzi pochopitelne do switche dejte volani funkci a ne primo vykonny kod

	switch (regs.rax.l) {
			case kiv_os::scCreate_File: {
				Create_File(regs);
			}
			break;	//scCreateFile


		case kiv_os::scWrite_File: {
				Write_File(regs);

			}
			break; //scWriteFile

		case kiv_os::scRead_File: {
			Read_File(regs);

		}
			break; //scReadFile

		case kiv_os::scDelete_File: {
			Delete_File(regs);
		}
			break;

		case kiv_os::scSet_File_Position: {
			Set_File_Position(regs);
		}
			break;

		case kiv_os::scGet_File_Position: {
			Get_File_Position(regs);
		}
			break;

		case kiv_os::scClose_Handle: {
				HANDLE hnd = Resolve_kiv_os_Handle(regs.rdx.x);

				regs.flags.carry = !close_file((f_des *)hnd) == S_OK;
				if (!regs.flags.carry) Remove_Handle(regs.rdx.x);				
					else regs.rax.r = GetLastError();
			}

			break;	//CloseFile
		
		case kiv_os::scGet_Current_Directory: {
			Get_Current_Directory(regs);

		}
			break;

		case kiv_os::scSet_Current_Directory: {
			Set_Current_Directory(regs);
		}
			break;

		case kiv_os::scCreate_Pipe: {
				Create_Pipe(regs);

			}
			break;
		
	}
}

void Create_File(kiv_os::TRegisters &regs) {
	
	
	HANDLE result;
	if (regs.rcx.l != 0) {
		result = open_file((char*)regs.rdx.r, true);
	}
	else {
		result = open_file((char*)regs.rdx.r, false, regs.rcx.h);
	}

	//print_file_system();

	// TO DO: pøedìlat na file descriptor
	//HRESULT create_file = createChild(getRoot()->filePath, name, false);

	regs.flags.carry = result == INVALID_HANDLE_VALUE;

	if (!regs.flags.carry) {
		regs.rax.x = Convert_Native_Handle(result); 
	}
	else {
		regs.rax.r = kiv_os::erInvalid_Handle;
	}
}

void Write_File(kiv_os::TRegisters &regs) {
	DWORD written;
	HANDLE hnd = Resolve_kiv_os_Handle(regs.rdx.x);
	regs.flags.carry = hnd == INVALID_HANDLE_VALUE;
//	if (!regs.flags.carry) regs.flags.carry = !WriteFile(hnd, reinterpret_cast<void*>(regs.rdi.r), (DWORD)regs.rcx.r, &written, NULL);
	if (!regs.flags.carry) {
		regs.flags.carry = write_file(reinterpret_cast<f_des*>(hnd), reinterpret_cast<char*>(regs.rdi.r), static_cast<char>(regs.rcx.r));
		written = static_cast<char>(regs.rcx.r);
		if(hnd == stdout)
			fwrite(reinterpret_cast<char*>(regs.rdi.r), sizeof(char), regs.rcx.r, (FILE*)hnd);
		
	}
	if (!regs.flags.carry) regs.rax.r = static_cast<char>(regs.rcx.r);
	else regs.rax.r = GetLastError();

}

void Read_File(kiv_os::TRegisters &regs) {
	size_t read;
	HANDLE hnd = Resolve_kiv_os_Handle(regs.rdx.x);
	regs.flags.carry = hnd == INVALID_HANDLE_VALUE;
	//if (!regs.flags.carry) regs.flags.carry = !ReadFile(hnd, reinterpret_cast<void*>(regs.rdi.r), (DWORD)regs.rcx.r, &read, NULL);
	if (!regs.flags.carry) {
		regs.rax.r = (decltype(regs.rax.r))read_file(reinterpret_cast<f_des*>(hnd), reinterpret_cast<char*>(regs.rdi.r), reinterpret_cast<f_des*>(hnd)->act_pos, regs.rcx.r, read);
	}
	if (!regs.flags.carry) regs.rax.r = read;
	else regs.rax.r = GetLastError();

	if (hnd == stdin) {
		fgets(reinterpret_cast<char*>(regs.rdi.r), regs.rcx.r, stdin);
	}
	
}

void Delete_File(kiv_os::TRegisters &regs) {
	int result;
	FSystem *dir = find_child((char*)regs.rdx.r);

	if (dir != nullptr) {
		result = delete_child(dir);
		regs.rax.h = 0;
	}
	else {
		regs.rax.h = kiv_os::erFile_Not_Found;
	}
}

void Set_File_Position(kiv_os::TRegisters &regs) {
	HANDLE hnd = Resolve_kiv_os_Handle(regs.rdx.x);
	regs.flags.carry = hnd == INVALID_HANDLE_VALUE;
	
	if (!regs.flags.carry) {
		set_act_pos(reinterpret_cast<f_des*>(hnd), regs.rdi.r);

	}
}

void Get_File_Position(kiv_os::TRegisters &regs) {
	HANDLE hnd = Resolve_kiv_os_Handle(regs.rdx.x);
	regs.flags.carry = hnd == INVALID_HANDLE_VALUE;

	if (!regs.flags.carry) {
		regs.rax.e = act_pos(reinterpret_cast<f_des*>(hnd));

	}
}

void Get_Current_Directory(kiv_os::TRegisters &regs) {
	int i, j = 0;

	char *buffer = reinterpret_cast<char *>(regs.rdx.r);
	for (i = 0; i < PCB_SIZE; i++) {
		if (process_table[i] != nullptr && std::this_thread::get_id() == process_table[i]->thread.get_id()) {
			while (j < strlen(process_table[0]->path)) {
				buffer[j] = process_table[0]->path[j];
				j++;
			}
			buffer[j] = '\0';
			break;
		}
	}
	if (i == PCB_SIZE) {
		while (j < strlen(process_table[0]->path)) {
			buffer[j] = process_table[0]->path[j];
			j++;
		}
		buffer[j] = '\0';
	}
}

void Set_Current_Directory(kiv_os::TRegisters &regs) {
	int i, j = 0;
	char *buffer = reinterpret_cast<char *>(regs.rdx.r);
	for (i = 0; i < PCB_SIZE; i++) {
		if (process_table[i] != nullptr && std::this_thread::get_id() == process_table[i]->thread.get_id()) {
			set_actual_node(find_child(buffer));
			process_table[i]->path = actual_node()->filePath.c_str();
			break;
		}
	}

	if (i == PCB_SIZE) {
		FSystem *node = find_child(buffer);
		if(node != nullptr){
			set_actual_node(node);
		}
		else {
			printf("Directory '%s' doesn't exists.\n", buffer);
		}
		process_table[0]->path = actual_node()->filePath.c_str();
		
	}

}

void Create_Pipe(kiv_os::TRegisters &regs) {
	
}