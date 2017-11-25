#include "../api/api.h"
#include "rd.h"
#include "rtl.h"
#include "parser.h"


size_t __stdcall rd(const kiv_os::TRegisters &regs) {
	//systemove volani do jadra
	char arg[10][1025];
	int argc;
	kiv_os::TProcess_Startup_Info* rd_info = reinterpret_cast<kiv_os::TProcess_Startup_Info*>(regs.rdx.r);
	size_t isDir = 1;

	parse_args(arg, &argc, rd_info->arg, str_len(rd_info->arg));

	if (argc == 2) {
		char * dir_name = arg[1];
		kiv_os_rtl::Delete_File(dir_name, isDir);
		if (regs.rax.h == 0) {

		}
		else {
			//error
		}
	}
	else {
		//error
	}

	return 0;
}