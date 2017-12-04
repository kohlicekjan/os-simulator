#include "../api/api.h"
#include "type.h"
#include "rtl.h"
#include "parser.h"


size_t __stdcall type(const kiv_os::TRegisters &regs) {
	kiv_os::TProcess_Startup_Info* process_info = reinterpret_cast<kiv_os::TProcess_Startup_Info *>(regs.rdx.r);

	char *arg = process_info->arg;

	kiv_os::THandle std_in = process_info->stdin;
	kiv_os::THandle std_out = process_info->stdout;
	kiv_os::THandle std_err = process_info->stderr;

	char file_name[2][1025];
	int argc;
	char args[1025];
	char *buffer = new char[100];
	size_t written = 0;
	kiv_os::THandle file;
	parse_args(file_name, &argc, arg, str_len(arg));
	if (file_name[argc - 1][0] == 0) {
		argc--;
	}

	//z pipy
	if (argc == 1) {
		file = std_in;
	}
	else {
		//zkusi otevrit soubor
		file = kiv_os_rtl::Create_File(file_name[argc - 1], 5);
	}

	//nechcem parsovat mezery
	parse_echo(args, arg, str_len(arg));
	
	//invalid handle == vypis stdin
	if (file == kiv_os::erInvalid_Handle) {
		kiv_os_rtl::Write_File(std_out, args, str_len(args), written);
	}
	else {
		//vypis file
		while(true){
			kiv_os_rtl::Read_File(file, buffer, 100, &written);
			if (written == 0) {
				kiv_os_rtl::Write_File(std_out, "\n", str_len("\n"), written);
				break;
			}
			kiv_os_rtl::Write_File(std_out, buffer, str_len(buffer), written);
		}
		if (file != std_in) {
			kiv_os_rtl::Close_File(file);
		}
	}


	return 0;
}