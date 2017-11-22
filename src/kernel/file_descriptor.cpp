#pragma once

#include "file_descriptor.h"
#include "file_system.h"
#include <mutex>

f_des *open_file(std::string path, bool isDir, uint64_t mode) {

	f_des *descriptor = new f_des;
	descriptor->open_type = mode;
	descriptor->act_pos = 0;
	descriptor->writing = true;
	descriptor->reading = true;
	
	std::string name = split_string(path).back();
	FSystem *file = find_child(path);

	if (WRITE == mode) {
		descriptor->reading = false;
		if (file != nullptr) {
			file->content = EOF;
		}
		else {
			file = create_child(path.substr(0, path.length() - name.length()), name, isDir);
		}

	} else if(READ == mode) {
		descriptor->writing = false;
		if (file != nullptr) {
			return  (f_des *)INVALID_HANDLE_VALUE;
		}
		else {
			file = create_child(path.substr(0, path.length() - name.length()), name, isDir);
		}

	} else if (WRITE_UPDATE == mode) {
		if (file != nullptr) {
			file->content = EOF;
		}
		else {
			file = create_child(path.substr(0, path.length() - name.length()), name, isDir);
		}

	} else if (READ_UPDATE == mode) {
		if (file == nullptr) {
			return (f_des *) INVALID_HANDLE_VALUE;
		}

	} else if (APPEND == mode) {
		descriptor->reading = false;
		if (file == nullptr) {
			file = create_child(path.substr(0, path.length() - name.length()), name, isDir);
		}
		descriptor->act_pos = file->content.length() - 1;

	} else if (APPEND_UPDATE == mode) {
		if (file == nullptr) {
			file = create_child(path.substr(0, path.length() - name.length()), name, isDir);
		}
		descriptor->act_pos = file->content.length() - 1;

	} else {
		return  (f_des *)INVALID_HANDLE_VALUE;
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


bool is_EOF(f_des *des) {
	int pos;
	if (des != nullptr) {
		pos = des->act_pos;
		return (des->file->content.at(pos) == EOF);
	}

	return false;
}


char read_file(f_des *des) {
	if (!des->reading) {
		return -4;
	}

	char next_char = -2;
	if (des->file->isDirectory) {
		return -3;
	}

	if (des->file != nullptr) {
		next_char = des->file->content.at(des->act_pos);
		if (!is_EOF(des)) {
			des->act_pos++;
		}
	}

	return next_char;
}

char *read_file(f_des *des, char *buffer, int start_pos, int size_to_read) {
	if (!des->reading) {		
		return nullptr;
	}

	if (des->file->isDirectory) {
		return nullptr;
	}
	
	//TODO: otestovat zda se hodnota nezmìní po pøedání funkci
	buffer = (char *)des->file->content.substr(start_pos, start_pos + size_to_read).c_str();
	
	return buffer;
}

//zapisuje na konec souboru
HRESULT write_file(f_des *des, char *to_write, char size_to_write) {
	if (!des->writing) {
		return E_INVALIDARG;
	}
	
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
	if (!des->writing) {
		return E_INVALIDARG;
	}

	if (des->file->isDirectory) {
		return E_INVALIDARG;
	}

	des->file->content.pop_back(); //Odstranìní EOF
	std::string pom = des->file->content.substr(0, start_pos);
	pom.append(buffer);
	pom.append(des->file->content.substr(start_pos, des->file->content.length()));
	des->file->content = pom;
	des->file->content.push_back(EOF);

	return S_OK;
}

HRESULT close_file(f_des *des) {
	if (des != nullptr) {
		delete(des);
	}
	
	return S_OK;
}

HRESULT remove_file(f_des *des) {
	if (des != nullptr) {
		delete_child(des->file); //TODO: otestovat zda se zaznam aktualizuje
		delete (des);
	}
	return S_OK;
}