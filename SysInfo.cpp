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
			tasks.push_back(_get_data(proc + runner->d_name));
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

Task	SysInfo::_get_data(std::string const path) const
{
	Task			t;
	std::ifstream	fstat(path + "/stat");
	std::vector<std::string> stat_data{std::istream_iterator<std::string>(fstat),
		std::istream_iterator<std::string>()};

	t.set_pid(stat_data[0]);
	t.set_name(stat_data[1]);
	t.set_state(stat_data[2]);
	t.set_nice(stat_data[18]);
	t.set_priority(stat_data[17]);

	unsigned long int vmsize = std::stoul(stat_data[22]) / 1024;
	t.set_vmsize(std::to_string(vmsize)); // replace by PAGE_SIZE

	long int	res = std::stol(stat_data[23]) * 4096 / 1024;
	t.set_res(std::to_string(res));

	std::ifstream	fstatus(path + "/statm");
	std::vector<std::string> statm_data{std::istream_iterator<std::string>(fstatus),
		std::istream_iterator<std::string>()};

	long int	shr = std::stol(statm_data[2]) * 4096 / 1024;
	t.set_shr(std::to_string(shr));

	struct	stat	st;
	if (lstat(path.c_str(), &st))
		throw std::logic_error("Can't lstat dir");

	struct passwd	*ps;
	ps = getpwuid(st.st_uid);
	t.set_user(ps->pw_name);

	unsigned long	utime = std::stoul(stat_data[13]);
	unsigned long	stime = std::stoul(stat_data[14]);
	double	timep = (utime + stime) / 100.0;
	t.set_timep(std::to_string(timep)); // meybe store in ticks

	double	mem = (static_cast<double>(res) / get_mem_total()) * 100.0 + 0.05;
	t.set_mem(std::to_string(mem));
	return (t);
}
