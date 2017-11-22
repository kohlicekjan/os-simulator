#pragma once

#include "io.h"
#include "kernel.h"
#include "handles.h"
#include "stdio.h"
#include "file_descriptor.h"
#include "file_system.h"


void HandleIO(kiv_os::TRegisters &regs) {

	//V ostre verzi pochopitelne do switche dejte volani funkci a ne primo vykonny kod

	switch (regs.rax.l) {
			case kiv_os::scCreate_File: {
				Create_File(regs);
			}
			break;	//scCreateFile


		case kiv_os::scWrite_File: {
				DWORD written;
				HANDLE hnd = Resolve_kiv_os_Handle(regs.rdx.x);
				regs.flags.carry = hnd == INVALID_HANDLE_VALUE;
				if (!regs.flags.carry) regs.flags.carry = !WriteFile(hnd, reinterpret_cast<void*>(regs.rdi.r), (DWORD)regs.rcx.r, &written, NULL);
				if (!regs.flags.carry) regs.rax.r = written;
					else regs.rax.r = GetLastError();

			}
			break; //scWriteFile

		case kiv_os::scRead_File: {
			DWORD read;
			HANDLE hnd = Resolve_kiv_os_Handle(regs.rdx.x);
			regs.flags.carry = hnd == INVALID_HANDLE_VALUE;
			if (!regs.flags.carry) regs.flags.carry = !ReadFile(hnd, reinterpret_cast<void*>(regs.rdi.r), (DWORD)regs.rcx.r, &read, NULL);
			if (!regs.flags.carry) regs.rax.r = read;
			else regs.rax.r = GetLastError();

		}
			break; //scReadFile

		case kiv_os::scClose_Handle: {
				HANDLE hnd = Resolve_kiv_os_Handle(regs.rdx.x);

				regs.flags.carry = !close_file((f_des *)hnd) == S_OK;
				if (!regs.flags.carry) Remove_Handle(regs.rdx.x);				
					else regs.rax.r = GetLastError();
			}

			break;	//CloseFile
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

	print_file_system();

	// TO DO: pøedìlat na file descriptor
	//HRESULT create_file = createChild(getRoot()->filePath, name, false);

	regs.flags.carry = result == INVALID_HANDLE_VALUE;

	if (!regs.flags.carry) {
		regs.rax.x = Convert_Native_Handle(result); 
	}
	else {
		regs.rax.r = GetLastError();
	}
}

