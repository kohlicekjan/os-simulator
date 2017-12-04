#include "../api/api.h"
#include "echo.h"
#include "rtl.h"
#include "parser.h"

size_t __stdcall echo(const kiv_os::TRegisters &regs) {
	//systemove volani do jadra
	kiv_os::TProcess_Startup_Info* process_info = reinterpret_cast<kiv_os::TProcess_Startup_Info *>(regs.rdx.r);

	char *arg = process_info->arg;

	kiv_os::THandle std_in = process_info->stdin;
	kiv_os::THandle std_out = process_info->stdout;
	kiv_os::THandle std_err = process_info->stderr;

	char args[1025];
	size_t written;
	
	parse_echo(args, arg, str_len(arg));

	kiv_os_rtl::Write_File(std_out, args, str_len(args), written);

	return 0;
}