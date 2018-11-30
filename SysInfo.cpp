#include "SysInfo.hpp"
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <pwd.h>
#include <numeric>
#include <math.h>
#include <sys/stat.h>

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

unsigned long int	get_cpu_total()
{
	std::ifstream				ftotal_stat("/proc/stat");
	std::vector<std::string>	total{std::istream_iterator<std::string>(ftotal_stat),
		std::istream_iterator<std::string>()};

	return ((unsigned long int)std::accumulate(total.begin() + 1, total.begin() + 11, 0UL,
				[](unsigned long int fst, std::string &str2) { return (fst + std::stol(str2));  }));
}

unsigned long int	get_mem_total()
{
	std::ifstream	fmeminfo{"/proc/meminfo"};
	std::vector<std::string> meminfo_data{std::istream_iterator<std::string>(fmeminfo),
		std::istream_iterator<std::string>()};
	unsigned long int	res;

	res = std::stol(*(std::find(meminfo_data.begin(), meminfo_data.end(), "MemTotal:") + 1));
	return (res);
}

Task	SysInfo::get_data(std::string const path) const
{
	Task			t;
	struct	stat	st;
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

	if (lstat(path.c_str(), &st))
		throw std::logic_error("Can't lstat dir");
	struct passwd	*ps;
	ps = getpwuid(st.st_uid);
	t.set_user(ps->pw_name);

	t.set_timep(std::to_string((std::stod(stat_data[13]) + std::stod(stat_data[14])) / 100.0)); // meybe store in ticks
	std::cout << (std::stod(t.get_res()) / get_mem_total()) * 100.0 + 0.05 << std::endl;
	t.set_mem(std::to_string(std::round((std::stod(stat_data[23]) / get_mem_total()) * 100.0)));
	return (t);
}
