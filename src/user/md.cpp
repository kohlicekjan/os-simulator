#include "../api/api.h"
#include "md.h"
#include "parser.h"
#include "rtl.h"


size_t __stdcall md(const kiv_os::TRegisters &regs) {
	//systemove volani do jadra

	char arg[10][1025];
	int argc;
	kiv_os::TProcess_Startup_Info* md_info = reinterpret_cast<kiv_os::TProcess_Startup_Info*>(regs.rdx.r);
	size_t flags = 1;
	size_t isDir = 1;

	parse_args(arg, &argc, md_info->arg, str_len(md_info->arg));
	
	if(argc == 2){
		char * dir_name = arg[1];
		kiv_os_rtl::Create_File(dir_name, flags, isDir);
	}
	else {
		//error
	}

	return 0;
}