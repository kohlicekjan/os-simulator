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

FSystem *createRoot();
FSystem *getRoot();
void printFileSystem();
void printFiles(FSystem *dir, int depth);
void printVisualDepth(int depth, FSystem *file);
std::vector<std::string> split_string(std::string str);
HRESULT createChild(std::string path, std::string name, bool isDirectory);
FSystem *getActualNode();
FSystem *findChild(std::string path);
int deleteChild(std::string path);
void deleteSubdirs(FSystem *dir);
bool childExist(FSystem *parent, std::string newChild);
void setActualNode(FSystem *node);
