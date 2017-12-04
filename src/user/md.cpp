#include "../api/api.h"
#include "md.h"
#include "parser.h"
#include "rtl.h"


size_t __stdcall md(const kiv_os::TRegisters &regs) {
	//systemove volani do jadra

	char arg[10][1025];
	int argc;
	kiv_os::TProcess_Startup_Info* md_info = reinterpret_cast<kiv_os::TProcess_Startup_Info*>(regs.rdx.r);
	size_t flags = 2;
	size_t isDir = 1;
	size_t written;

	kiv_os::THandle std_out = md_info->std_out;

	parse_args(arg, &argc, md_info->arg, str_len(md_info->arg));
	
	if(argc == 2){
		char * dir_name = arg[1];
		kiv_os::THandle new_dir = kiv_os_rtl::Create_File(dir_name, flags, isDir);
		if (new_dir == kiv_os::erInvalid_Handle) {
			char buffer[100];
			buffer[0] = '\0';
			str_cat(buffer, "A directory or file '");
			str_cat(buffer, dir_name);
			str_cat(buffer, "' already exists.\n");
			kiv_os_rtl::Write_File(std_out, buffer, str_len(buffer), written);
		}
	}
	else {
		kiv_os_rtl::Write_File(std_out, "Invalid argument!\n", str_len("Invalid argument!\n"), written);
	}

	return 0;
}