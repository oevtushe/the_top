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

SysInfo::SysInfo()
{
	update();
}

std::vector<SysInfo::Procinfo> const	&SysInfo::get_procs_data() const
{
	return (_proc);
}

SysInfo::Cpuinfo const					&SysInfo::get_cpu_data() const
{
	return (_cpu);
}

SysInfo::Meminfo const					&SysInfo::get_mem_data() const
{
	return (_mem);
}

SysInfo::Load_avg const					&SysInfo::get_loadavg() const
{
	return (_load_avg);
}

std::string const						&SysInfo::get_curtime() const
{
	return (_cur_time);
}

long int								SysInfo::get_uptime() const
{
	return (_uptime);
}

int										SysInfo::get_num_of_users() const
{
	return (_num_of_users);
}

SysInfo::Tasks_count const						&SysInfo::get_tasks_count() const
{
	return (_tasks_count);
}

std::vector<SysInfo::Procinfo>			SysInfo::_read_proc_data() const
{
	DIR						*dir;
	struct dirent			*runner;
	std::vector<Procinfo>	db;
	std::string				proc{"/proc/"};
	std::string				wrap;

	if (!(dir = opendir(proc.c_str())))
		throw std::logic_error("Can't open proc");
	while ((runner = readdir(dir)))
	{
		wrap = runner->d_name;
		if (std::all_of(wrap.begin(), wrap.end(), [](char c){ return std::isdigit(c); }))
			db.push_back(_read_proc_data_hlp(proc + runner->d_name));
	}
	closedir(dir);
	return (db);
}

SysInfo::Procinfo						SysInfo::_read_proc_data_hlp(std::string const path) const //local
{
	std::ifstream	fstat(path + "/stat");
	std::vector<std::string> stat_data{std::istream_iterator<std::string>(fstat),
		std::istream_iterator<std::string>()};
	Procinfo		pi{};

	pi.pid = std::stoi(stat_data[0]);
	pi.command = stat_data[1];
	pi.state = stat_data[2][0];
	pi.nice = std::stol(stat_data[18]);
	pi.priority = std::stol(stat_data[17]);
	pi.vsize = std::stoul(stat_data[22]) / 1024;
	pi.rss = std::stol(stat_data[23]) * getpagesize() / 1024;

	std::ifstream	fstatus(path + "/statm");
	std::vector<std::string> statm_data{std::istream_iterator<std::string>(fstatus),
		std::istream_iterator<std::string>()};
	pi.mem_shared = std::stol(statm_data[2]) * getpagesize() / 1024;

	struct	stat	st;
	if (lstat(path.c_str(), &st))
		throw std::logic_error("Can't lstat dir");
	pi.user = getpwuid(st.st_uid)->pw_name;

	unsigned long int utime = std::stoul(stat_data[13]);
	unsigned long int stime = std::stoul(stat_data[14]);
	pi.timep = (utime + stime);/// sysconf(_SC_CLK_TCK);

	pi.memp = (static_cast<double>(pi.rss) / _mem.mem_total) * 100.0;
	pi.cpu = utime + stime;
	return (pi);
}

long int				SysInfo::_read_uptime() const //general
{
	long int			up{};
	struct sysinfo		si;

	sysinfo(&si);
	up = si.uptime;
	return (up);
}

SysInfo::Meminfo		SysInfo::_read_mem_data(std::ifstream &fmemi) const //general
{
	Meminfo						mi;
	std::vector<std::string>	mi_data{std::istream_iterator<std::string>(fmemi),
		std::istream_iterator<std::string>()};

	//mi.bc = std::stoul(mi_data[10]) + std::stoul(mi_data[67]) + std::stoul(mi_data[13]);
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
	//unsigned long int			used{};
	//buffers = std::stoul(mi_data[10]) + std::stoul(mi_data[13]) + std::stoul(mi_data[67]);
	//used = ttl - free - buffers;
	return (mi);
}

SysInfo::Load_avg	SysInfo::_read_loadavg_data(std::ifstream &flavg) const //general
{
	Load_avg					la;
	std::vector<std::string>	raw{std::istream_iterator<std::string>(flavg),
		std::istream_iterator<std::string>()};
	la.la_1 = std::stod(raw[0]);
	la.la_5 = std::stod(raw[1]);
	la.la_15 = std::stod(raw[2]);
	return (la);
}

SysInfo::Cpuinfo				SysInfo::_read_cpu_data(std::ifstream &fstat) const //general
{
	std::vector<std::string>	stat{std::istream_iterator<std::string>(fstat),
										std::istream_iterator<std::string>()};
	Cpuinfo						ci{};

	ci.user = std::stol(stat[1]);
	ci.nice = std::stol(stat[2]);
	ci.system = std::stol(stat[3]);
	ci.idle = std::stol(stat[4]);
	ci.iowait = std::stol(stat[5]);
	ci.irq = std::stol(stat[6]);
	ci.softirq = std::stol(stat[7]);
	ci.steal = std::stol(stat[8]);

	//long int					cpu_total = std::accumulate(stat.begin() + 1, stat.begin() + 11, 0L,
	//		[](long int fst, std::string &str2) { return (fst + std::stol(str2));  });
	//ci.total = cpu_total;
	return (ci);
}

int					SysInfo::_read_num_of_users() const
{
	FILE		*f{};
	struct utmp tmp;
	int			cnt{};

	if (!(f = fopen("/run/utmp", "r")))
		throw(std::logic_error("/run/utmp no such file, or i can't open it"));
	while (fread(&tmp, sizeof(tmp), 1, f))
		++cnt;
	fclose(f);
	return (cnt);
}

std::string			SysInfo::_read_cur_time() const
{
	char	buff[9];
	time_t	tloc;
	struct tm *tmp{};

	tloc = time(NULL);
	tmp = localtime(&tloc);
	if (!tmp)
		throw (std::logic_error("Can't get time"));
	if (!strftime(buff, sizeof(buff), "%T", tmp))
		throw (std::logic_error("strftime goes down"));
	return (buff);
}

/*
void			SysInfo::_calc_cpu_usage()
{
}
*/

void			SysInfo::_calc_tasks()
{
	_tasks_count.total = _proc.size();
	_tasks_count.running = std::count_if(_proc.begin(), _proc.end(), [](SysInfo::Procinfo const &t){ return (t.state == 'R');});
	_tasks_count.sleeping = std::count_if(_proc.begin(), _proc.end(), [](SysInfo::Procinfo const &t){ return (t.state == 'S');});
	_tasks_count.stopped = std::count_if(_proc.begin(), _proc.end(), [](SysInfo::Procinfo const &t){ return (t.state == 'T');});
	_tasks_count.zombie = std::count_if(_proc.begin(), _proc.end(), [](SysInfo::Procinfo const &t){ return (t.state == 'Z');});
}

void			SysInfo::update()
{
	std::ifstream	fstat("/proc/stat");
	std::ifstream	floadavg("/proc/loadavg");
	std::ifstream	fmemi("/proc/meminfo");
	std::ifstream	flavg("/proc/loadavg");

	_cpu = _read_cpu_data(fstat);
	_mem = _read_mem_data(fmemi);
	_proc = _read_proc_data();
	_load_avg = _read_loadavg_data(flavg);
	_uptime = _read_uptime();
	_num_of_users = _read_num_of_users();
	_cur_time = _read_cur_time();
	//_calc_cpu_usage();
	_calc_tasks();
}
