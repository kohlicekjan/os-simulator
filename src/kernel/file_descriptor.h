#pragma once

#include <vector>
#include <string>
#include <windows.h>
#include "file_system.h"

typedef struct file_descriptor {
	int act_pos;
	std::string filename;
	FSystem *file;
	std::string path;
	int open_type;
	int open_clients;
}f_des;


f_des *open_file(std::string path);
char read_file(f_des *des); //read one byte
char *read_file(f_des *des, char *buffer, int start_pos, int size_to_read);
HRESULT write_file(f_des *des, char *to_write, char size_to_write);
HRESULT write_file(f_des *des, char *buffer, int start_pos, int size_to_read);
HRESULT close_file(f_des *des);
HRESULT remove_file(f_des *des);
