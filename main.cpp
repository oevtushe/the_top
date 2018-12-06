#include "SysInfo.hpp"
#include <utmp.h>
#include <sys/sysinfo.h>
#include <ncurses.h>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <unistd.h>
#include <fstream>
#include <algorithm>

int		get_users()
{
	FILE *f{};
	struct utmp tmp;
	int		cnt{};

	if (!(f = fopen("/run/utmp", "r")))
		throw(std::logic_error("/run/utmp no such file, or i can't open it"));
	while (fread(&tmp, sizeof(tmp), 1, f))
		++cnt;
	fclose(f);
	return (cnt);
}

void	display_gen_info(SysInfo::GenDB &db, SysInfo::ProcessesDB &pdb)
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

	struct sysinfo si;

	sysinfo(&si);
	long int	hours = si.uptime / 3600;
	long int	minutes = (si.uptime % 3600) / 60;
	std::ifstream	favg("/proc/loadavg");
	std::vector<std::string> lavg_data{std::istream_iterator<std::string>(favg),
		std::istream_iterator<std::string>()};
	mvprintw(0, 0, "the_top - %s up %2ld:%ld, %2.2s user,  load average: %s, %s, %s\n",
			buff,
			hours,
			minutes,
			std::to_string(get_users()).c_str(),
			lavg_data[0].c_str(),
			lavg_data[1].c_str(),
			lavg_data[2].c_str());
	int		total = pdb.size();
	int		running = std::count_if(pdb.begin(), pdb.end(), [](SysInfo::ProcessDB &t){ return (t["state"] == "R");});
	int		sleeping = std::count_if(pdb.begin(), pdb.end(), [](SysInfo::ProcessDB &t){ return (t["state"] == "S");});
	int		stopped = std::count_if(pdb.begin(), pdb.end(), [](SysInfo::ProcessDB &t){ return (t["state"] == "T");});
	int		zombie = std::count_if(pdb.begin(), pdb.end(), [](SysInfo::ProcessDB &t){ return (t["state"] == "Z");});
	mvprintw(1, 0, "Tasks: %3d total, %3d running, %3d sleeping, %3d stopped, %3d zombie\n",
			total,
			running,
			sleeping,
			stopped,
			zombie);
	///////////////////////////////////////////////////////////////////
	mvprintw(2, 0, "%%Cpu(s): %s us, %s sy, %s ni, %s id, %s wa, %s hi, %s si, %s st\n",
			db["pus"].c_str(),
			db["psy"].c_str(),
			db["pni"].c_str(),
			db["npid"].c_str(),
			db["pwa"].c_str(),
			db["phi"].c_str(),
			db["psi"].c_str(),
			db["pst"].c_str());
	///////////////////////////////////////////////////////////////////
	std::ifstream	fmi("/proc/meminfo");
	std::vector<std::string> mi_data{std::istream_iterator<std::string>(fmi),
		std::istream_iterator<std::string>()};
	unsigned long int	buffers{};
	unsigned long int	ttl{std::stoul(mi_data[1])};
	unsigned long int	free{std::stoul(mi_data[4])};
	unsigned long int	used{};
	buffers = std::stoul(mi_data[10]) + std::stoul(mi_data[13]) + std::stoul(mi_data[67]);
	used = ttl - free - buffers;
	mvprintw(3, 0, "KiB Mem : %8.8s total, %8.8s free, %8.8s used, %8.8s buff/cache\n",
			mi_data[1].c_str(),
			mi_data[4].c_str(),
			std::to_string(used).c_str(),
			std::to_string(buffers).c_str());

	unsigned long int swap_total = std::stoul(mi_data[43]);
	unsigned long int swap_free = std::stoul(mi_data[46]);
	unsigned long int swap_used = swap_total - swap_free;
	mvprintw(4, 0, "KiB Swap: %8lu total, %8lu free, %8lu used. %8.8s avail Mem\n",
			swap_total,
			swap_free,
			swap_used,
			mi_data[7].c_str());
	mvprintw(5, 0, "%5.5s %-9.9s %2.2s %3.3s %7.7s %7.7s %7.7s %1.1s %4.4s %4.4s %9.9s %.7s\n",
			"PID",
			"USER",
			"PR",
			"NI",
			"VIRT",
			"RES",
			"SHR",
			"S",
			"%CPU",
			"%MEM",
			"TIME+",
			"COMMAND");
}

void	display_proc_info(SysInfo::ProcessesDB &db)
{
	move(6, 0);
	for (auto &proc : db)
	{
		printw("%5.5s %-9.9s %2.2s %3.3s %7.7s %7.7s %7.7s %1.1s %4.4s %4.4s %9.7s %-s\n",
				proc["pid"].c_str(),
				proc["user"].c_str(),
				proc["priority"].c_str(),
				proc["nice"].c_str(),
				proc["vmsize"].c_str(),
				proc["res"].c_str(),
				proc["shared"].c_str(),
				proc["state"].c_str(),
				"%CPU",
				proc["mem"].c_str(),
				proc["time+"].c_str(),
				proc["command"].c_str());
	}
}

struct	prev_state
{
	unsigned long int		us;
	unsigned long int		sy;
	unsigned long int		ni;
	unsigned long int		id;
	unsigned long int		wa;
	unsigned long int		hi;
	unsigned long int		si;
	unsigned long int		st;
	unsigned long int		total;
};

int		main(void)
{
	SysInfo	si;
	SysInfo::ProcessesDB	proc;
	SysInfo::GenDB			gen;
	struct prev_state ps{};
	struct prev_state ps_dif{};

	initscr();
	start_color();
	curs_set(0);

	while (true)
	{
		proc = si.get_tasks();
		gen = si.get_gen_data();
		ps_dif.us = std::stoul(gen["us"]) - ps.us;
		ps_dif.sy = std::stoul(gen["sy"]) - ps.sy;
		ps_dif.ni = std::stoul(gen["ni"]) - ps.ni;
		ps_dif.id = std::stoul(gen["id"]) - ps.id;
		ps_dif.wa = std::stoul(gen["wa"]) - ps.wa;
		ps_dif.hi = std::stoul(gen["hi"]) - ps.hi;
		ps_dif.si = std::stoul(gen["si"]) - ps.si;
		ps_dif.st = std::stoul(gen["st"]) - ps.st;
		ps_dif.total = ps_dif.us + ps_dif.sy + ps_dif.ni + ps_dif.id + ps_dif.wa + ps_dif.hi + ps_dif.si + ps_dif.st;
		gen["pus"] = std::to_string((static_cast<double>(ps_dif.us) / ps_dif.total) * 100.0);
		gen["psy"] = std::to_string((static_cast<double>(ps_dif.sy) / ps_dif.total) * 100.0);
		gen["pni"] = std::to_string((static_cast<double>(ps_dif.ni) / ps_dif.total) * 100.0);
		gen["npid"] = std::to_string((static_cast<double>(ps_dif.id) / ps_dif.total) * 100.0);
		gen["pwa"] = std::to_string((static_cast<double>(ps_dif.wa) / ps_dif.total) * 100.0);
		gen["phi"] = std::to_string((static_cast<double>(ps_dif.hi) / ps_dif.total) * 100.0);
		gen["psi"] = std::to_string((static_cast<double>(ps_dif.si) / ps_dif.total) * 100.0);
		gen["pst"] = std::to_string((static_cast<double>(ps_dif.st) / ps_dif.total) * 100.0);
		display_gen_info(gen, proc);
		display_proc_info(proc);
		refresh();
		sleep(1);
		erase();
		ps.us = std::stoul(gen["us"]);
		ps.sy = std::stoul(gen["sy"]);
		ps.ni = std::stoul(gen["ni"]);
		ps.id = std::stoul(gen["id"]);
		ps.wa = std::stoul(gen["wa"]);
		ps.hi = std::stoul(gen["hi"]);
		ps.si = std::stoul(gen["si"]);
		ps.st = std::stoul(gen["st"]);
		ps.total = ps.us + ps.sy + ps.ni + ps.id + ps.wa + ps.hi + ps.si + ps.st;
	}
	getch();
	endwin();
	return (0);
}
