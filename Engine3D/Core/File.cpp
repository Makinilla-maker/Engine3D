#include "File.h"
#include "ModuleFileSystem.h"


void File::Read()
{
	std::vector<std::string> direc;

	App->fileSystem->DiscoverFiles(this->path.c_str(), this->files, direc);

	if (direc.size() > 0)
	{
		for (uint i = 0; i < direc.size(); i++)
		{
			std::string _path = this->path + std::string("/") + direc.at(i) + std::string("/");
			App->fileSystem->file = new File(direc.at(i).c_str());
			App->fileSystem->file->path = _path;
			this->childs.push_back(App->fileSystem->file);
			App->fileSystem->file->owner = this;
			App->fileSystem->file->Read();
		}
	}
	for (uint i = 0; i < this->files.size(); i++)
	{
		File* h = new File(this->files.at(i));
		h->owner = this;
		this->childs.push_back(h);
	}

	direc.clear();

}