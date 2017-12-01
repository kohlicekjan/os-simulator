#include "../api/api.h"
#include "wc.h"
#include "rtl.h"
#include "parser.h"


size_t __stdcall wc(const kiv_os::TRegisters &regs) {
	kiv_os::TProcess_Startup_Info* process_info = reinterpret_cast<kiv_os::TProcess_Startup_Info *>(regs.rdx.r);

	char *arg = process_info->arg;

	kiv_os::THandle std_in = process_info->std_in;
	kiv_os::THandle std_out = process_info->std_out;
	kiv_os::THandle std_err = process_info->std_err;

	char args[1025];
	char *buffer = new char[100];
	size_t written = 0;

	parse_echo(args, arg, str_len(arg));

	kiv_os::THandle neco = kiv_os_rtl::Create_File(args, 1);
	kiv_os_rtl::Write_File(neco, "blablablabla", str_len("blablablabla"), written);

	kiv_os::THandle file = kiv_os_rtl::Create_File(args, 5);

	//vypis error
	if (file == kiv_os::erInvalid_Handle) {
		str_cpy(args, "Invalid Handle!\n", str_len("Invalid Handle!\n"));
		kiv_os_rtl::Write_File(std_out, args, str_len(args), written);
	}
	else {
		//vypis file
		while (true) {
			kiv_os_rtl::Read_File(file, buffer, 100, &written);
			if (written == 0) {			
				break;
			}
		}
	}

	return 0;
}