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
#include <sys/sysinfo.h>
#include <unistd.h>
#include <utmp.h>

bool									operator==(ISys::Procinfo_raw const &a,
												ISys::Procinfo_raw const &b)
{
	return (a.pid == b.pid);
}

SysInfo::SysInfo()
{
	update();
}

std::vector<ISys::Procinfo_raw>			SysInfo::_read_proc_data() const
{
	DIR								*dir;
	struct dirent					*runner;
	std::vector<ISys::Procinfo_raw>	db;
	std::string						wrap;
	std::string						proc{"/proc/"};

	if (!(dir = opendir(proc.c_str())))
		throw std::logic_error("Can't open proc");
	while ((runner = readdir(dir)))
	{
		wrap = runner->d_name;
		if (std::all_of(wrap.begin(), wrap.end(), [](char c){ return std::isdigit(c); }))
		{
			std::string		path{proc + runner->d_name};
			std::ifstream	fs{path + "/stat"};
			std::ifstream	fsm{path + "/statm"};
			if (fs.good() && fsm.good())
				db.push_back(_read_proc_data_hlp(fs, fsm, path));
		}
	}
	closedir(dir);
	return (db);
}

ISys::Procinfo_raw						SysInfo::_read_proc_data_hlp(std::ifstream &fstat, std::ifstream &fsm, std::string const &path) const
{
	std::vector<std::string>	stat_data{std::istream_iterator<std::string>(fstat),
		std::istream_iterator<std::string>()};
	ISys::Procinfo_raw			pi{};

	pi.pid = std::stoi(stat_data[0]);
	pi.command = stat_data[1];
	pi.state = stat_data[2][0];
	pi.nice = std::stol(stat_data[18]);
	pi.priority = std::stol(stat_data[17]);
	pi.vsize = std::stoul(stat_data[22]) / 1024;
	pi.rss = std::stol(stat_data[23]) * getpagesize() / 1024;

	std::vector<std::string> statm_data{std::istream_iterator<std::string>(fsm),
		std::istream_iterator<std::string>()};
	pi.mem_shared = std::stol(statm_data[2]) * getpagesize() / 1024;

	struct	stat	st;
	if (lstat(path.c_str(), &st))
		throw std::logic_error("Can't lstat dir " + path);
	pi.user = getpwuid(st.st_uid)->pw_name;

	unsigned long int utime = std::stoul(stat_data[13]);
	unsigned long int stime = std::stoul(stat_data[14]);
	pi.timep = (utime + stime);

	pi.memp = (static_cast<double>(pi.rss) / _mem.mem_total) * 100.0;
	pi.cpu = utime + stime;
	return (pi);
}

long int								SysInfo::_read_uptime() const
{
	long int		up{};
	struct sysinfo	si;

	sysinfo(&si);
	up = si.uptime;
	return (up);
}

ISys::Meminfo							SysInfo::_read_mem_data() const
{
	std::ifstream				fmemi("/proc/meminfo");
	if (fmemi.fail())
		throw (std::logic_error("/proc/meminfo unavailable"));
	ISys::Meminfo				mi;
	std::vector<std::string>	mi_data{std::istream_iterator<std::string>(fmemi),
		std::istream_iterator<std::string>()};

	mi.mem_total = std::stoul(mi_data[1]);

	unsigned long int buffer = std::stoul(mi_data[10]);
	unsigned long int cache = std::stoul(mi_data[13]);
	unsigned long int sreclaimable = std::stoul(mi_data[67]);
	mi.bc = buffer + cache + sreclaimable;

	mi.mem_free = std::stoul(mi_data[4]);
	mi.swap_total = std::stoul(mi_data[43]);
	mi.swap_free = std::stoul(mi_data[46]);
	mi.available = std::stoul(mi_data[7]);
	mi.mem_used = mi.mem_total - mi.mem_free - mi.bc;
	mi.swap_used = mi.swap_total - mi.swap_free;
	return (mi);
}

ISys::Load_avg							SysInfo::_read_loadavg_data() const
{
	std::ifstream				flavg("/proc/loadavg");
	if (flavg.fail())
		throw (std::logic_error("/proc/loadavg unavailable"));
	ISys::Load_avg				la;
	std::vector<std::string>	raw{std::istream_iterator<std::string>(flavg),
		std::istream_iterator<std::string>()};
	la.la_1 = std::stod(raw[0]);
	la.la_5 = std::stod(raw[1]);
	la.la_15 = std::stod(raw[2]);
	return (la);
}

ISys::Cpuinfo							SysInfo::_read_cpu_data() const
{
	std::ifstream				fstat("/proc/stat");
	if (fstat.fail())
		throw (std::logic_error("/proc/stat unavailable"));
	std::vector<std::string>	stat{std::istream_iterator<std::string>(fstat),
										std::istream_iterator<std::string>()};
	ISys::Cpuinfo				ci{};

	ci.user = std::stol(stat[1]);
	ci.nice = std::stol(stat[2]);
	ci.system = std::stol(stat[3]);
	ci.idle = std::stol(stat[4]);
	ci.iowait = std::stol(stat[5]);
	ci.irq = std::stol(stat[6]);
	ci.softirq = std::stol(stat[7]);
	ci.steal = std::stol(stat[8]);
	return (ci);
}

int										SysInfo::_read_num_of_users() const
{
	FILE		*f{};
	struct utmp tmp;
	int			cnt{};

	if (!(f = fopen("/run/utmp", "r")))
		throw(std::logic_error("/run/utmp unavailable"));
	while (fread(&tmp, sizeof(tmp), 1, f))
		++cnt;
	fclose(f);
	return (cnt);
}

std::string								SysInfo::_read_cur_time() const
{
	char		buff[9];
	time_t		tloc;
	struct tm	*tmp{};

	tloc = time(NULL);
	tmp = localtime(&tloc);
	if (!tmp)
		throw (std::logic_error("Can't get time"));
	if (!strftime(buff, sizeof(buff), "%T", tmp))
		throw (std::logic_error("strftime goes down"));
	return (buff);
}

ISys::Tasks_count		SysInfo::_calc_tasks()
{
	ISys::Tasks_count	tc{};

	tc.total = _proc.size();
	tc.running = std::count_if(_proc.begin(), _proc.end(),
			[](SysInfo::Procinfo_raw const &t){ return (t.state == 'R');});
	tc.sleeping = std::count_if(_proc.begin(), _proc.end(),
			[](SysInfo::Procinfo_raw const &t){ return (t.state == 'S');});
	tc.stopped = std::count_if(_proc.begin(), _proc.end(),
			[](SysInfo::Procinfo_raw const &t){ return (t.state == 'T');});
	tc.zombie = std::count_if(_proc.begin(), _proc.end(),
			[](SysInfo::Procinfo_raw const &t){ return (t.state == 'Z');});
	return (tc);
}
