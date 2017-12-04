#pragma once

#include <vector>
#include <string>
#include <windows.h>
#include "file_system.h"

static int WRITE = 1; //just writing - create empty file  - cursor at beginning
static int READ = 2; //just read - file must exist
static int APPEND = 3; //writing - file we be created - cursor at end
static int WRITE_UPDATE = 4; //both - new file will be created
static int READ_UPDATE = 5; //both - file must exist
static int APPEND_UPDATE = 6; //both - file is created if dont exist 

typedef struct file_descriptor {
	int act_pos;
	std::string filename;
	FSystem *file;
	uint64_t open_type;
	bool reading;
	bool writing;
}f_des;


f_des *open_file(std::string path, bool isDir = false, uint64_t mode = WRITE);

int act_pos(f_des *des);
void set_act_pos(f_des *des, int pos);

bool is_EOF(f_des *des);

char read_file(f_des *des); //read one byte
char *read_file(f_des *des, char buffer[], int start_pos, int size_to_read, size_t &read);

HRESULT write_file(f_des *des, char *to_write, char size_to_write);
HRESULT write_file(f_des *des, char *buffer, int start_pos, int size_to_read);

HRESULT close_file(f_des *des);
HRESULT remove_file(f_des *des);
