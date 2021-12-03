#pragma once
#include <vector>
#include <string>
#include "Globals.h"
#include "Application.h"

class File
{
public:
	File(const std::string _name) 
	{
		name = _name;
	}

	~File() {}

	void Read();

	File* owner = nullptr;
	
	std::string name;
	std::string path;

	std::vector<File*> childs;
	std::vector<std::string> files;

	bool selected = false;
};