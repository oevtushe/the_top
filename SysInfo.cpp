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

SysInfo::ProcessesDB	SysInfo::get_tasks() const
{
	DIR					*dir;
	struct dirent		*runner;
	ProcessesDB			db;
	std::string			proc{"/proc/"};
	std::string			wrap;

	if (!(dir = opendir(proc.c_str())))
		throw std::logic_error("Can't open proc");
	while ((runner = readdir(dir)))
	{
		wrap = runner->d_name;
		if (std::all_of(wrap.begin(), wrap.end(), [](char c){ return std::isdigit(c); }))
			db.push_back(_get_proc_data(proc + runner->d_name));
	}
	closedir(dir);
	return (db);
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

SysInfo::GenDB		SysInfo::get_gen_data() const
{
	GenDB	db;
	std::ifstream				ftotal_stat("/proc/stat");
	std::vector<std::string>	total{std::istream_iterator<std::string>(ftotal_stat),
		std::istream_iterator<std::string>()};

	unsigned long int cpu_total = std::accumulate(total.begin() + 1, total.begin() + 11, 0UL,
				[](unsigned long int fst, std::string &str2) { return (fst + std::stol(str2));  });

	db["cpu_total"] = std::to_string(cpu_total);
	db["us"] = total[1];
	db["ni"] = total[2];
	db["sy"] = total[3];
	db["id"] = total[4];
	db["wa"] = total[5];
	db["hi"] = total[6];
	db["si"] = total[7];
	db["st"] = total[8];
	return (db);
}

SysInfo::ProcessDB	SysInfo::_get_proc_data(std::string const path) const
{
	ProcessDB		data;
	std::ifstream	fstat(path + "/stat");
	std::vector<std::string> stat_data{std::istream_iterator<std::string>(fstat),
		std::istream_iterator<std::string>()};

	data["pid"] = stat_data[0];
	data["command"] = stat_data[1];
	data["state"] = stat_data[2];
	data["nice"] = stat_data[18];
	data["priority"] = stat_data[17];

	unsigned long int vmsize = std::stoul(stat_data[22]) / 1024;
	data["vmsize"] = std::to_string(vmsize); // replace by PAGE_SIZE

	long int	res = std::stol(stat_data[23]) * 4096 / 1024;
	data["res"] = std::to_string(res);

	std::ifstream	fstatus(path + "/statm");
	std::vector<std::string> statm_data{std::istream_iterator<std::string>(fstatus),
		std::istream_iterator<std::string>()};
	long int	shr = std::stol(statm_data[2]) * 4096 / 1024;
	data["shared"] = std::to_string(shr);

	struct	stat	st;
	if (lstat(path.c_str(), &st))
		throw std::logic_error("Can't lstat dir");
	data["user"] = getpwuid(st.st_uid)->pw_name;

	unsigned long	utime = std::stoul(stat_data[13]);
	unsigned long	stime = std::stoul(stat_data[14]);
	double	timep = (utime + stime) / 100.0;
	data["time+"] = std::to_string(timep); // meybe store in ticks

	double	mem = (static_cast<double>(res) / get_mem_total()) * 100.0 + 0.05;
	data["mem"] = std::to_string(mem);
	//data["us"] = std::to_string(stat_data[13]);
	//data["sy"] = std::to_string(stat_data[14]);
	return (data);
}
