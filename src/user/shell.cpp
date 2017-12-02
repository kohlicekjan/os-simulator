
#include "shell.h"
#include "rtl.h"
#include "parser.h"
#include <stdio.h>

size_t __stdcall shell(kiv_os::TRegisters &regs) {
	/*kiv_os::THandle std_out_ex = kiv_os_rtl::Create_File("C://system/term/CONOUT$", 1);	//nahradte systemovym resenim, zatim viz Console u CreateFile na MSDN
	kiv_os::THandle std_in_ex = kiv_os_rtl::Create_File("C://system/term/CONIN$", 2);	//nahradte systemovym resenim, zatim viz Console u CreateFile na MSDN
	kiv_os::THandle std_err_ex = kiv_os_rtl::Create_File("C://system/term/CONERR$", 1);	//nahradte systemovym resenim, zatim viz Console u CreateFile na MSDN */

	kiv_os::TProcess_Startup_Info* shell_info = reinterpret_cast<kiv_os::TProcess_Startup_Info *>(regs.rdx.r);

	kiv_os::THandle std_out = shell_info->std_out;
	kiv_os::THandle std_in = shell_info->std_in;
	kiv_os::THandle std_err = shell_info->std_err;

	kiv_os::TProcess_Startup_Info process_info;
	
	size_t written;
	char * buf_command = new char[1025];
	bool run = true;

	char * cur_path = new char[1025];
	size_t buffer_size = 1025;
	char command_part[1025];
	char command_name[1025];
	bool name_loaded = false;
	int command_argc = 0;
	int input_size = 0;
	int i, size = 0;

	while (true) {
		
		kiv_os_rtl::Get_Current_Directory(cur_path, buffer_size);
		size = str_len(cur_path);
		if (size < 1025) {
			cur_path[size++] = '>';
			cur_path[size] = '\0';
		}
		else {
			cur_path[1023] = '>';
			cur_path[1024] = '\0';
		}
		
		kiv_os_rtl::Write_File(std_out, cur_path, str_len(cur_path, 1025), written);
		size_t filled;
		
		kiv_os_rtl::Read_File(std_in, buf_command, 1025, &filled);
		char *input = buf_command;
		
		process_info.std_in = std_in;
		process_info.std_out = std_out;
		process_info.std_err = std_err;
		regs.rdi.r = (decltype(regs.rdi.r))&process_info;

		kiv_os::THandle pipe_in;
		kiv_os::THandle pipe_out;

		input_size = str_len(input);
		if (input[0] == 26) {
			break;
		}

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
			char arg[256][1025];
			int argc;
			parse_args(arg, &argc, command_part, str_len(command_part));

			if (input_cmp(arg[argc - 2], 2, ">>", 2)) {
				process_info.std_out = kiv_os_rtl::Create_File(arg[argc - 1], 6);
				//vymazani presmerovani z argumentù pøíkazu, 3 je poèet mezer
				command_part[str_len(command_part) - 3 - str_len(arg[argc - 1]) - str_len(arg[argc - 2])] = '\0';
			}
			else if (input_cmp(arg[argc - 2], 1, ">", 1)) {
				process_info.std_out = kiv_os_rtl::Create_File(arg[argc - 1], 3);
				//vymazani presmerovani z argumentù pøíkazu, 3 je poèet mezer
				command_part[str_len(command_part) - 3 - str_len(arg[argc - 1]) - str_len(arg[argc - 2])] = '\0';
			}
			if (input_cmp(arg[argc - 2], 3, "2>>", 3)) {
				process_info.std_err = kiv_os_rtl::Create_File(arg[argc - 1], 6);
				//vymazani presmerovani z argumentù pøíkazu, 3 je poèet mezer
				command_part[str_len(command_part) - 3 - str_len(arg[argc - 1]) - str_len(arg[argc - 2])] = '\0';
			}
			else if (input_cmp(arg[argc - 2], 2, "2>", 2)) {
				process_info.std_err = kiv_os_rtl::Create_File(arg[argc - 1], 3);
				//vymazani presmerovani z argumentù pøíkazu, 3 je poèet mezer
				command_part[str_len(command_part) - 3 - str_len(arg[argc - 1]) - str_len(arg[argc - 2])] = '\0';
			}
			
			//vzdy pro shell udelat
			if (input_cmp(command_name, str_len(command_name), "shell", str_len("shell"), true)) {
				process_info.std_in = std_in;
			}

			//TADY DODELAT DIR
			if (input_cmp(command_name, str_len(command_name), "dir", str_len("dir"), true)) {
				char buffer[100];
				written = 0;
				if (argc == 2) {
					kiv_os_rtl::Set_Current_Directory(arg[1]);
				}

				//potreba vratit handle na slozku
				kiv_os::THandle dir = kiv_os_rtl::Create_File(cur_path, 2);

				while (true) {
					//precte obsah adresare
					kiv_os_rtl::Read_File(dir, buffer, 100, &written);
					if (written == 0) {
						break;
					}
					//vypise na konzoli
					kiv_os_rtl::Write_File(std_out, buffer, str_len(buffer), written);
				}
			}

			if (input_cmp(command_name, str_len(command_name), "cd", str_len("cd"), true) && argc == 2) {
				kiv_os_rtl::Set_Current_Directory(arg[1]);
			}
			else {
				regs.rdx.r = (decltype(regs.rdx.r))command_name;
				process_info.arg = command_part;
				if (kiv_os_rtl::Create_Process(regs) == true) {
					//aby cekal na ctrl+z na stdin
					if (input_cmp(command_name, str_len(command_name), "rgen", str_len("rgen"))) {
						regs.rdx.r = std_in;
					}
					else {
						regs.rdx.r = 0;
					}
					kiv_os_rtl::Wait_For(regs);
				}
				
			}
		}

		
		
		name_loaded = false;
		command_argc = 0;
		
	}
	

	return 0;
}

void parser(char *buffer) {
}