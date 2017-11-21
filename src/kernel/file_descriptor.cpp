#include "file_descriptor.h"
#include "file_system.h"

f_des *open_file(std::string path, bool isDir ) {
	f_des *descriptor = new f_des;
	descriptor->act_pos = 0;
	FSystem *file = findChild(path);
	std::string name = split_string(path).back();

	if (file == nullptr) {
		file = createChild(path, name, isDir);
	}
	descriptor->filename = name;
	descriptor->file = file;
	return descriptor;
}

int act_pos(f_des *des) {
	if (des != nullptr) {
		return des->act_pos;
	}
	
	return -1;
}

void set_act_pos(f_des *des, int pos) {
	if (des != nullptr && des->file->content.length() > pos) {
		des->act_pos = pos;
	}
}


bool isEOF(f_des *des) {
	int pos;
	if (des != nullptr) {
		pos = des->act_pos;
		return (des->file->content.at(pos) == EOF);
	}

	return false;
}


char read_file(f_des *des) {

	char next_char = '\0';
	if (des->file->isDirectory) {
		return -1;
	}

	if (des->file != nullptr) {
		next_char = des->file->content.at(des->act_pos);
		if (des->file->content.size() != (des->act_pos - 1)) {
			des->act_pos++;
		}
		
	}
	return next_char;
}

char *read_file(f_des *des, char *buffer, int start_pos, int size_to_read) {
	if (des->file->isDirectory) {
		return nullptr;
	}


	//TODO: otestovat zda se hodnota nezm�n� po p�ed�n� funkci
	buffer = (char *)des->file->content.substr(start_pos, start_pos + size_to_read).c_str();
	
	return buffer;
}

//zapisuje na konec souboru
HRESULT write_file(f_des *des, char *to_write, char size_to_write) {
	std::string pom;
	if (des->file->isDirectory) {
		return E_INVALIDARG;
	}
	des->file->content.pop_back();
	des->file->content.append(to_write);
	des->file->content.push_back(EOF);
	return S_OK;
}

HRESULT write_file(f_des *des, char *buffer, int start_pos, int size_to_write) {
	if (des->file->isDirectory) {
		return E_INVALIDARG;
	}

	des->file->content.pop_back(); //Odstran�n� EOF
	std::string pom = des->file->content.substr(0, start_pos);
	pom.append(buffer);
	pom.append(des->file->content.substr(start_pos, des->file->content.length()));
	des->file->content = pom;
	des->file->content.push_back(EOF);

	return S_OK;
}

HRESULT close_file(f_des *des) {
	delete(des);
	return S_OK;
}

HRESULT remove_file(f_des *des) {
	deleteChild(des->file); //TODO: otestovat zda se zaznam aktualizuje
	delete (des);
	return S_OK;
}