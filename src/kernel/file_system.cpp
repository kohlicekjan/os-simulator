
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
	root->filePath = "C://";
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
	
	child->isDirectory = isDirectory;
	child->owner = "";

	child->parent = find_child(path, true);
	if (child->parent == nullptr) {
		return nullptr;
	}

	
	if (child->parent->parent == nullptr) {
		child->filePath = child->parent->filePath;
	}
	else {
		child->filePath = child->parent->filePath;
		
	}

	if (isDirectory) {
		child->filePath +=  name + "/";
	}


	if (child_exist(child->parent, name)) {
		return nullptr;
	}
	
	//naplneni obsahu slozky
	if (isDirectory) {
		child->parent->content += "\t<DIR>\t" + name + "\n";
	}
	else {
		child->parent->content += "\t<FILE>\t" + name  + "\n";
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
	node->parent->content = "";

	for (childIt = node->parent->children.begin(); childIt != node->parent->children.end(); childIt++) {
		if ((*childIt)->filename.compare(node->filename) == 0) {
			node->parent->children.erase(childIt);
			break;
		}
		else {
			if ((*childIt)->isDirectory) {
				node->parent->content += "\t<DIR>\t" + (*childIt)->filename + "\n";
			}
			else {
				node->parent->content += "\t<FILE>\t" + (*childIt)->filename + "\n";
			}
		}

	}

	delete_subdirs(node);

	return 2;
}

/// <summary> function for deleting subdirs - is called from function deleteChild
/// <param name='dir'>node to delete</param>
/// </summary>  
bool delete_subdirs(FSystem *dir) {
	std::vector<FSystem *>::iterator childIt;
	FSystem *parent = dir->parent;

	for (childIt = dir->children.begin(); childIt != dir->children.end(); childIt++) {
		if ((*childIt)->isDirectory) {
			delete_subdirs((*childIt));
		}
		else {
			delete((*childIt));
		}
	}

	

	delete(dir);
	return true;
}


/// <summary> searching for node in the path
/// <param name='path'>path to the node - last element will be returned</param>
/// </summary> 
FSystem *find_child(std::string path, bool hasLock) {

	if (!hasLock) {
		std::lock_guard<std::mutex> lock(fs_lock);
	}
	

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
