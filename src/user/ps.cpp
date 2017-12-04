#include "../api/api.h"
#include "ps.h"
#include "rtl.h"
#include "parser.h"

size_t __stdcall ps(kiv_os::TRegisters &regs) {
	//systemove volani do jadra
	
	kiv_os::TProcess_Startup_Info* process_info = reinterpret_cast<kiv_os::TProcess_Startup_Info *>(regs.rdx.r);

	char *arg = process_info->arg;
	
	kiv_os::THandle std_in = process_info->stdin;	
	kiv_os::THandle std_out = process_info->stdout;
	kiv_os::THandle std_err = process_info->stderr;
	
	char *buffer = new char[100];
	size_t written = 0;

	str_cpy(buffer, "PID\t TID\t Name \t\t Working directory\n", str_len("PID\t TID\t Name \t\t Working directory\n"));
	kiv_os_rtl::Write_File(std_out, buffer, str_len(buffer), written);

	while(true){
		kiv_os_rtl::Read_File(std_in, buffer, 100, &written);
		if (written == 0) {
			break;
		}
		kiv_os_rtl::Write_File(std_out, buffer, str_len(buffer), written);
	}
	
	return 0; 
}