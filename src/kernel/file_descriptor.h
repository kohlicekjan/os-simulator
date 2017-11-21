#pragma once

#include <vector>
#include <string>
#include <windows.h>
#include "file_system.h"

const char *WRITE = "w"; //just writing - create empty file  - cursor at beginning
const char *READ = "r"; //just read - file must exist
const char *APPEND = "a"; //writing - file we be created - cursor at end
const char *WRITE_UPDATE = "w+"; //both - new file will be created
const char *READ_UPDATE = "r+"; //both - file must exist
const char *APPEND_UPDATE = "a+"; //both - file is created if dont exist 

typedef struct file_descriptor {
	int act_pos;
	std::string filename;
	FSystem *file;
	std::string open_type;
	int open_clients;
	bool reading;
	bool writing;
}f_des;


f_des *open_file(std::string path, bool isDir = false, const char *mode = WRITE);

int act_pos(f_des *des);
void set_act_pos(f_des *des, int pos);

bool is_EOF(f_des *des);

char read_file(f_des *des); //read one byte
char *read_file(f_des *des, char *buffer, int start_pos, int size_to_read);

HRESULT write_file(f_des *des, char *to_write, char size_to_write);
HRESULT write_file(f_des *des, char *buffer, int start_pos, int size_to_read);

HRESULT close_file(f_des *des);
HRESULT remove_file(f_des *des);
