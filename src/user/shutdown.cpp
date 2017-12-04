#include "../api/api.h"
#include "shutdown.h"
#include "rtl.h"


size_t __stdcall shutdown(const kiv_os::TRegisters &regs) {
	//systemove volani do jadra
	kiv_os::TProcess_Startup_Info* process_info = reinterpret_cast<kiv_os::TProcess_Startup_Info *>(regs.rdx.r);

	kiv_os::THandle std_in = process_info->stdin;
	kiv_os::THandle std_out = process_info->stdout;
	kiv_os::THandle std_err = process_info->stderr;

	return 0;
}