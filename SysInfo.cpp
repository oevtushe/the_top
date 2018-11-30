#include "SysInfo.hpp"
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>

std::vector<Task> SysInfo::get_tasks() const
{
	DIR					*dir;
	struct dirent		*runner;
	std::vector<Task>	tasks;
	std::string			proc{"/proc/"};
	std::string			wrap;

	if (!(dir = opendir(proc.c_str())))
		throw std::logic_error("Can't open proc");
	while ((runner = readdir(dir)))
	{
		wrap = runner->d_name;
		if (std::all_of(wrap.begin(), wrap.end(), [](char c){ return std::isdigit(c); }))
			tasks.push_back(get_data(proc + runner->d_name));
	}
	closedir(dir);
	return (tasks);
}

Task	SysInfo::get_data(std::string const path) const
{
	Task			t;
	std::ifstream	fstat(path + "/stat");
	std::ifstream	fstatus(path + "/statm");
	std::vector<std::string> stat_data{std::istream_iterator<std::string>(fstat),
		std::istream_iterator<std::string>()};
	std::vector<std::string> statm_data{std::istream_iterator<std::string>(fstatus),
		std::istream_iterator<std::string>()};

	t.set_pid(stat_data[0]);
	t.set_name(stat_data[1]);
	t.set_state(stat_data[2]);
	t.set_nice(stat_data[18]);
	t.set_priority(stat_data[17]);
	t.set_vmsize(std::to_string(std::stoi(stat_data[22]) / 1024)); // replace by PAGE_SIZE
	t.set_shr(std::to_string(std::stoi(statm_data[2]) * 4096 / 1024));
	t.set_res(std::to_string(std::stoi(stat_data[23]) * 4096 / 1024));
	return (t);
}
