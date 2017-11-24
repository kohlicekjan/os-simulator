
#include "shell.h"
#include "rtl.h"
#include "parser.h"
#include <stdio.h>

size_t __stdcall shell(kiv_os::TRegisters &regs) {
	kiv_os::THandle std_out = kiv_os_rtl::Create_File("C://system/term/CONOUT$", 1);	//nahradte systemovym resenim, zatim viz Console u CreateFile na MSDN
	kiv_os::THandle std_in = kiv_os_rtl::Create_File("C://system/term/CONIN$", 2);	//nahradte systemovym resenim, zatim viz Console u CreateFile na MSDN
	kiv_os::THandle std_err = kiv_os_rtl::Create_File("C://system/term/CONERR$", 1);	//nahradte systemovym resenim, zatim viz Console u CreateFile na MSDN


	kiv_os::TProcess_Startup_Info process_info;
	
	size_t written;
	char * buf_command = new char[256];
	bool run = true;

	char * cur_path = new char[256];
	size_t buffer_size = 256;
	char command_part[1025];
	char command_name[1025];
	bool name_loaded = false;
	int command_argc = 0;
	int input_size = 0;
	int i;

	kiv_os_rtl::Set_Current_Directory("C://system/term/");

	while (true) {
		kiv_os_rtl::Get_Current_Directory(cur_path, buffer_size);
	//	printf("%s\n", cur_path);
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
		
		process_info.std_in = std_in;
		process_info.std_out = std_out;
		process_info.std_err = std_err;
		regs.rdi.r = (decltype(regs.rdi.r))&process_info;

		kiv_os::THandle pipe_in;
		kiv_os::THandle pipe_out;

		input_size = str_len(input);
		for (i = 0; i <= input_size; i++) {
			if (input[i] == '|') {
				command_part[command_argc] = '\0';
				process_info.arg = command_part;
				regs.rdx.r = (decltype(regs.rdx.r))command_name;

				kiv_os_rtl::Create_Process(regs);

			//	kiv_os_rtl::Create_Pipe(process_info.std_in, process_info.std_out);
				
				command_argc = 0;
				name_loaded = false;
				i++; // preskoci mezeru za |
			}
			else {
				if ((input[i] == ' ' || input[i] == 10 ) && !name_loaded) {
					name_loaded = true;
					command_name[command_argc] = '\0';

				}else if (!name_loaded) {
					command_name[command_argc] = input[i];
				}
				
				if (input[i] != 10) {
					command_part[command_argc++] = input[i];
				}
			}
		}

		if (name_loaded) {
			command_part[input_size] = '\0';
		}
		else {
			command_name[i] = '\0';
		}

		if (str_len(command_name) >= 2) {
			regs.rdx.r = (decltype(regs.rdx.r))command_name;
			process_info.arg = command_part;
			kiv_os_rtl::Create_Process(regs);
		}


		/*
		
			DOPORUÈENÍ - POUŽIJ FUNKCI V PARSER.CPP PARSE_ARGS() PRO PARSOVÁNÍ ØÁDKU S ARGUMENTY
		*/
		
		name_loaded = false;
		command_argc = 0;
		
	}
	kiv_os_rtl::Close_File(std_in);
	kiv_os_rtl::Close_File(std_out);
	kiv_os_rtl::Close_File(std_err);
	return 0;
}

void parser(char *buffer) {
}