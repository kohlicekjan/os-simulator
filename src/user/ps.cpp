#include "../api/api.h"
#include "ps.h"
#include "rtl.h"
#include "parser.h"
#include <stdio.h>

size_t __stdcall ps(kiv_os::TRegisters &regs) {
	//systemove volani do jadra
	
	kiv_os::TProcess_Startup_Info* process_info = reinterpret_cast<kiv_os::TProcess_Startup_Info *>(regs.rdx.r);

	char *arg = process_info->arg;
	
	kiv_os::THandle std_in = process_info->std_in;	
	kiv_os::THandle std_out = process_info->std_out;
	kiv_os::THandle std_err = process_info->std_err;
	
	if (kiv_os_rtl::Return_PCB(regs) == true) {
		
		int process_size = regs.rcx.e;
		int* pids = reinterpret_cast<int*>(regs.rdi.r);

		size_t written = 0;

		char* process_table = reinterpret_cast<char *>(regs.rdx.r);
		//char *process;
		int i = 0;
		
		//kiv_os_rtl::Write_File(std_out, process_table, process_size, written);

		/* WTF? */
	//	printf("%d\n", pids[2]);
	//	printf("%d\n", pids[2]);
		
		/*
		while (pids[i] != -1) {
			printf("%d\n", pids[i]);
			i++;
		}*/
		
	}
	
	return 0; 
}