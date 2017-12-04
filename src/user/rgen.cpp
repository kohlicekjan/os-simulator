#include "../api/api.h"
#include "rgen.h"
#include "rtl.h"
#include "parser.h"

size_t __stdcall rgen(kiv_os::TRegisters &regs) {

	kiv_os::TProcess_Startup_Info* process_info = reinterpret_cast<kiv_os::TProcess_Startup_Info *>(regs.rdx.r);
	int pid = (int)regs.rcx.r;
	char *arg = process_info->arg;

	kiv_os::THandle std_in = process_info->std_in;
	kiv_os::THandle std_out = process_info->std_out;
	kiv_os::THandle std_err = process_info->std_err;

	char buffer[100];
	char pom[10];
	size_t written;
	int r;
	bool run = true;

	while(run){
		buffer[0] = '\0';
		r = rand();		
		atoi(r, pom);
		str_cat(buffer, pom);
		str_cat(buffer, ".");
		r = rand();
		atoi(r, pom);
		str_cat(buffer, pom);
		str_cat(buffer, " ");
		regs.rax.r = 0;
		regs.rcx.r = 100;
		regs.rdi.r = pid;
		run = kiv_os_rtl::Wait_For(regs);
		if (run == false) {	
			break;
		}
		kiv_os_rtl::Write_File(std_out, buffer, str_len(buffer), written);
	}

	return 0;
}

unsigned short lfsr = 0xACE1u;
unsigned bit;

unsigned rand()
{
	bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
	return lfsr = (lfsr >> 1) | (bit << 15);
}