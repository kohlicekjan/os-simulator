#pragma once


#include <vector>
#include <string>
#include <windows.h>


const bool FS_DIRECTORY = true;
const bool FS_FILE = false;

typedef struct file_system {
	std::string filename;
	std::string owner;
	file_system *parent;
	std::vector <file_system*> children;
	std::string content;
	bool isDirectory;
	std::string filePath;
}FSystem;

FSystem *create_root();
FSystem *get_root();
void print_file_system();
void print_files(FSystem *dir, int depth);
void print_visual_depth(int depth, FSystem *file);
std::vector<std::string> split_string(std::string str);
FSystem *create_child(std::string path, std::string name, bool isDirectory);
FSystem *actual_node();
FSystem *find_child(std::string path, bool hasLock = false);
int delete_child(FSystem *node);
bool delete_subdirs(FSystem *dir);
bool child_exist(FSystem *parent, std::string newChild);
void set_actual_node(FSystem *node);
