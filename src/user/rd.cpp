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
	size_t written;
	char buffer[100];

	kiv_os::THandle std_out = rd_info->stdout;

	parse_args(arg, &argc, rd_info->arg, str_len(rd_info->arg));

	if (argc == 2) {
		char * dir_name = arg[1];
		bool result = kiv_os_rtl::Delete_File(dir_name, isDir);
		if (!result) {
			buffer[0] = '\0';
			str_cat(buffer, "Directory '");
			str_cat(buffer, dir_name);
			str_cat(buffer, "' doesn't found!\n");
			kiv_os_rtl::Write_File(std_out, buffer, str_len(buffer), written);
		}
	}
	else {
		kiv_os_rtl::Write_File(std_out, "Invalid argument!\n", str_len("Invalid argument!\n"), written);
	}

	return 0;
}