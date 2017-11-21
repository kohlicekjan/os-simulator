//
// Created by BobrZlosyn on 09.11.2017.
//

#include "file_system.h"

#include <iterator>
#include <regex>
#include <mutex>

//zamek pro operace, ktere meni strukturu stromu
std::mutex fs_lock;
FSystem *root = create_root(); // root node
FSystem *actualNode = get_root(); //node of actual directory

/// <summary> Creating new root  
/// </summary>  
FSystem *create_root() {
	FSystem *root = new FSystem;

	root->children = {};
	root->content = "";
	root->filename = "C:";
	root->filePath = "/";
	root->isDirectory = FS_DIRECTORY;
	root->owner = "";
	root->parent = nullptr;
	return root;
}

FSystem *get_root() {
	return root;
}

void set_actual_node(FSystem *node) {
	actualNode = node;
}

FSystem *actual_node() {
	return actualNode;
}


/// <summary> Creating child directory or file on given path
/// <param name='path'>path where will be node created</param>
/// <param name='name'>node name</param>
/// <param name='isDirectory'>true if node is directory</param>
/// </summary>  
FSystem *create_child(std::string path, std::string name, bool isDirectory) {
	std::lock_guard<std::mutex> lock(fs_lock);

	FSystem *child = new FSystem;
	child->children = {};
	child->content = EOF;
	child->filename = name;
	child->filePath = path;
	child->isDirectory = isDirectory;
	child->owner = "";

	child->parent = find_child(path);
	if (child->parent == nullptr) {
		return nullptr;
	}

	if (child_exist(child->parent, name)) {
		return nullptr;
	}

	child->parent->children.push_back(child);

	return child;
}


/// <summary> control if node with the given name already exist in folder
/// <param name='parent'>dir node which will be searched</param>
/// <param name='newChild'>name of new node</param>
/// </summary>  
bool child_exist(FSystem *parent, std::string newChild) {
	std::vector<FSystem *>::iterator childIt;
	for (childIt = parent->children.begin(); childIt != parent->children.end(); childIt++) {
		if ((*childIt)->filename.compare(newChild) == 0) {
			parent->parent->children.erase(childIt);
			return true;
		}
	}
	return false;
}


/// <summary> delete node on given path
/// <param name='path'> path to node, last element in path will be deleted</param>
/// </summary>  
int delete_child(FSystem *node) {
	std::lock_guard<std::mutex> lock(fs_lock);
	std::vector<FSystem *>::iterator childIt;

	if (node == nullptr || node->parent == nullptr) {
		return 1; //node wasnt found
	}

	for (childIt = node->parent->children.begin(); childIt != node->parent->children.end(); childIt++) {
		if ((*childIt)->filename.compare(node->filename) == 0) {
			node->parent->children.erase(childIt);
			break;
		}
	}

	delete_subdirs(node);
	return 2;
}

/// <summary> function for deleting subdirs - is called from function deleteChild
/// <param name='dir'>node to delete</param>
/// </summary>  
void delete_subdirs(FSystem *dir) {
	std::vector<FSystem *>::iterator childIt;
	for (childIt = dir->children.begin(); childIt != dir->children.end(); childIt++) {
		if ((*childIt)->isDirectory) {
			delete_subdirs((*childIt));
		}
		else {
			delete((*childIt));
		}
	}


	delete(dir);
}

/// <summary> function for deleting subdirs - is called from function deleteChild
/// <param name='dir'>node to delete</param>
/// </summary> 
void get_data(FSystem *file, size_t startPosition, size_t size, char** buffer, size_t *filled) {
	// TODO: need to be filled
}

/// <summary> function for deleting subdirs - is called from function deleteChild
/// <param name='dir'>node to delete</param>
/// </summary> 
void set_data(FSystem *file, char* buffer, size_t write) {
/*	if (file->isDirectory) {
		return;
	}
	//TODO: need to be tested 

	std::string tw("");
	for (size_t w = 0; w < write; w++) {
		tw += buffer[w];
	}

	file->content.append(tw);-*/
}


/// <summary> searching for node in the path
/// <param name='path'>path to the node - last element will be returned</param>
/// </summary> 
FSystem *find_child(std::string path) {

	std::lock_guard<std::mutex> lock(fs_lock);

	std::vector<std::string> pathParts;
	std::vector<std::string>::iterator it;
	std::vector<FSystem *>::iterator childIt;
	FSystem *startNode = actual_node();
	bool error = true;

	if (path.size() >= 2 && path.substr(0, 2).compare("./") == 0) {
		path = path.substr(2, path.size()); //setting right string for relative path
	}
	else if (path.size() >= 4 && path.substr(0, 4).compare("C://") == 0) {
		path = path.substr(4, path.size()); // setting absolute path
		startNode = get_root();
	}

	pathParts = split_string(path);
	if (pathParts.empty()) {
		return startNode;
	}

	for (it = pathParts.begin(); it != pathParts.end(); it++) {
		if ((*it).compare("..") == 0) {
			startNode = startNode->parent;
			if (startNode == nullptr) {
				return nullptr; //it went beyond root
			}
		}
		else {
			if (startNode->children.empty()) {
				return nullptr; // no children found
			}

			for (childIt = startNode->children.begin();
				childIt != startNode->children.end(); childIt++) {
				if ((*childIt)->filename.compare((*it)) == 0) {
					startNode = (*childIt);
					error = false;
					break; // next node found
				}
			}

			if (error) {
				return nullptr; // node wasnt found
			}

			error = true; //reseting error for next searching
		}
	}

	return startNode;
}

/// <summary> spliting path by "/"
/// <param name='str'>path to be split</param>
/// </summary> 
std::vector<std::string> split_string(std::string str) {
	char * str_c = &str[0];
	std::vector<std::string> path;
	std::string part;

	char* pch = NULL;
	char* context = NULL;
	char  delims[] = "/";
	pch = strtok_s(str_c, delims, &context);
	while (pch != NULL)
	{
		part = pch;
		path.push_back(part);
		pch = strtok_s(NULL, delims, &context);
	}
	return path;
}

/// <summary> printing whole file system 
/// </summary> 
void print_file_system() {
	FSystem *node = get_root();

	printf("\\ \n");
	print_files(node, 1);
}

/// <summary> printing structure with recurse call
/// <param name='dir'>dir where to start printing</param>
/// <param name='depth'>depth of the dir</param>
/// </summary> 
void print_files(FSystem *dir, int depth) {
	FSystem *pomNode;
	std::vector<FSystem*>::iterator it;
	std::vector<FSystem*> dirs;
	std::vector<FSystem*> childrens = dir->children;


	for (it = childrens.begin(); it != childrens.end(); it++) {
		pomNode = *it;
		if (pomNode->isDirectory) {
			dirs.push_back(pomNode);
			continue;
		}

		print_visual_depth(depth, pomNode);
	}


	for (it = dirs.begin(); it != dirs.end(); it++) {
		print_visual_depth(depth, *it);
		print_files(*it, depth + 1);
	}
}

/// <summary> print tabs to show depth
/// <param name='depth'>depth of the node</param>
/// <param name='file'>file to be printed</param>
/// </summary> 
void print_visual_depth(int depth, FSystem *file) {
	int i;
	for (i = 0; i < depth; i++) {
		printf("\t");
	}

	if (file->isDirectory) {
		printf("+ %s \n", file->filename.c_str());
	}
	else {
		printf("%s \n", file->filename.c_str());
	}

}
