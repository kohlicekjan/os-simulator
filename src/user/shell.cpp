
#include "shell.h"
#include "rtl.h"
#include "parser.h"


size_t __stdcall shell(kiv_os::TRegisters &regs) {
	kiv_os::THandle std_out = kiv_os_rtl::Create_File("C://system/term/CONOUT$", 1);	//nahradte systemovym resenim, zatim viz Console u CreateFile na MSDN
	kiv_os::THandle std_in = kiv_os_rtl::Create_File("C://system/term/CONIN$", 2);	//nahradte systemovym resenim, zatim viz Console u CreateFile na MSDN
	kiv_os::THandle std_err = kiv_os_rtl::Create_File("C://system/term/CONERR$", 1);	//nahradte systemovym resenim, zatim viz Console u CreateFile na MSDN


	kiv_os::TProcess_Startup_Info process_info;
	
	char* output = "Boot successful\n";
	
	size_t written;
	kiv_os_rtl::Write_File(std_out, output, 16, written);

	char * buf_command = new char[256];
	buf_command[255] = '\0';
	bool run = true;

	while (true) {
		char* cur_path = "C://";
		//std::string input_;
		kiv_os_rtl::Write_File(std_out, cur_path, str_len(cur_path, 256), written);
	//	std::getline(std::cin, input_);
		

		size_t filled;
		
		kiv_os_rtl::Read_File(std_in, buf_command, 256, &filled);
		
		/*
		if (filled == 0 && buf_command[filled] == -1) { //goodbye
			break;
		}
		buf_command[filled] = '\0';*/

		char *input = buf_command;
	//	char *input = (char *)input_.c_str();

		regs.rdx.r = (decltype(regs.rdx.r))input;
		process_info.std_in = std_in;
		process_info.std_out = std_out;
		process_info.std_err = std_err;
		regs.rdi.r = (decltype(regs.rdi.r))&process_info;

		if (input_cmp(input, str_len(input, 256), "ps", 8)) {	
			process_info.arg = "ps";
		}
		if (input_cmp(input, str_len(input, 256), "cd", 8)) {
			process_info.arg = "cd";
		}
		if (input_cmp(input, str_len(input, 256), "dir", 8)) {
			process_info.arg = "dir";
		}
		if (input_cmp(input, str_len(input, 256), "echo", 8)) {
			process_info.arg = "echo";
		}
		if (input_cmp(input, str_len(input, 256), "md", 8)) {
			process_info.arg = "md";
		}
		if (input_cmp(input, str_len(input, 256), "rd", 8)) {
			process_info.arg = "rd";
		}
		if (input_cmp(input, str_len(input, 256), "type", 8)) {
			process_info.arg = "type";
		}
		if (input_cmp(input, str_len(input, 256), "sort", 8)) {
			process_info.arg = "sort";
		}
		if (input_cmp(input, str_len(input, 256), "wc", 8)) {
			process_info.arg = "wc";
		}
		if (input_cmp(input, str_len(input, 256), "shutdown", 8)) {
			process_info.arg = "shutdown";
			break;
		}

		kiv_os_rtl::Create_Process(regs);		
		
	}

	kiv_os_rtl::Close_File(std_out);
	return 0;
}

void parser(char *buffer) {
}