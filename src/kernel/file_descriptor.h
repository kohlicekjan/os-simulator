#pragma once

#include <vector>
#include <string>
#include <windows.h>
#include "file_system.h"

typedef struct file_descriptor {
	int act_pos;
	std::string filename;
	FSystem *file;
	int open_type;
	int open_clients;
}f_des;


f_des *open_file(std::string path, bool isDir = false);

int act_pos(f_des *des);
void set_act_pos(f_des *des, int pos);

bool isEOF(f_des *des);

char read_file(f_des *des); //read one byte
char *read_file(f_des *des, char *buffer, int start_pos, int size_to_read);

HRESULT write_file(f_des *des, char *to_write, char size_to_write);
HRESULT write_file(f_des *des, char *buffer, int start_pos, int size_to_read);

HRESULT close_file(f_des *des);
HRESULT remove_file(f_des *des);
