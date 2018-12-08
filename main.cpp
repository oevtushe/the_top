#include "SysInfo.hpp"
#include <sys/sysinfo.h>
#include <ncurses.h>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <unistd.h>
#include <fstream>
#include <algorithm>

void	display_gen_info(SysInfo const &si)
{
	////////////////////////////////////////////////////////////// top part
	long int	hours = si.get_uptime() / 3600;
	long int	minutes = (si.get_uptime() % 3600) / 60;
	//////////////////////////////////////////////////////////////
	mvprintw(0, 0, "the_top - %s up %2ld:%ld, %2d user,  load average: %.2f, %.2f, %.2f\n",
			si.get_curtime().c_str(),
			hours,
			minutes,
			si.get_num_of_users(),
			si.get_loadavg()[0],
			si.get_loadavg()[1],
			si.get_loadavg()[2]);
	////////////////////////////////////////////////////////////// top part
	std::vector<SysInfo::Procinfo> proc = si.get_procs_data();
	int		total = proc.size();
	int		running = std::count_if(proc.begin(), proc.end(), [](SysInfo::Procinfo &t){ return (t.state == 'R');});
	int		sleeping = std::count_if(proc.begin(), proc.end(), [](SysInfo::Procinfo &t){ return (t.state == 'S');});
	int		stopped = std::count_if(proc.begin(), proc.end(), [](SysInfo::Procinfo &t){ return (t.state == 'T');});
	int		zombie = std::count_if(proc.begin(), proc.end(), [](SysInfo::Procinfo &t){ return (t.state == 'Z');});
	//////////////////////////////////////////////////////////////
	mvprintw(1, 0, "Tasks: %3d total, %3d running, %3d sleeping, %3d stopped, %3d zombie\n",
			total,
			running,
			sleeping,
			stopped,
			zombie);
	/////////////////////////////////////////////////////////////////// top part
	/*
	mvprintw(2, 0, "%%Cpu(s): %s us, %s sy, %s ni, %s id, %s wa, %s hi, %s si, %s st\n",
			db["pus"].c_str(),
			db["psy"].c_str(),
			db["pni"].c_str(),
			db["npid"].c_str(),
			db["pwa"].c_str(),
			db["phi"].c_str(),
			db["psi"].c_str(),
			db["pst"].c_str());
	*/
	///////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////// can be replaced
	SysInfo::Meminfo	memi = si.get_mem_data();
	unsigned long int	buffers{};
	unsigned long int	ttl{memi.mem_total};
	unsigned long int	free{memi.mem_free};
	unsigned long int	used{};
	buffers = memi.buffers + memi.cached + memi.sreclaimable;
	used = ttl - free - buffers;
	///////////////////////////////////////////////////////////////////
	mvprintw(3, 0, "KiB Mem : %8.8lu total, %8.8lu free, %8.8lu used, %8.8lu buff/cache\n",
			memi.mem_total,
			memi.mem_free,
			used,
			buffers);

	mvprintw(4, 0, "KiB Swap: %8lu total, %8lu free, %8lu used. %8.8lu avail Mem\n",
			memi.swap_total,
			memi.swap_free,
			memi.swap_total - memi.swap_free,
			memi.available);
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

void	display_proc_info(std::vector<SysInfo::Procinfo> const &procs)
{
	move(6, 0);
	for (auto &proc : procs)
	{
		printw("%5d %-9.9s %2d %3d %7d %7d %7d %c %4.4s %4d %9d %-s\n",
				proc.pid,
				proc.user.c_str(),
				proc.priority,
				proc.nice,
				proc.vsize,
				proc.rss,
				proc.mem_shared,
				proc.state,
				"%CPU",
				0, //
				0, //
				proc.command.c_str());
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
	SysInfo	prev_si;
	struct prev_state ps{};
	struct prev_state ps_dif{};

	initscr();
	start_color();
	curs_set(0);

	while (true)
	{
		/*
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
		*/
		si.update();
		display_gen_info(si);
		display_proc_info(si.get_procs_data());
		refresh();
		sleep(1);
		erase();
		/*
		ps.us = std::stoul(gen["us"]);
		ps.sy = std::stoul(gen["sy"]);
		ps.ni = std::stoul(gen["ni"]);
		ps.id = std::stoul(gen["id"]);
		ps.wa = std::stoul(gen["wa"]);
		ps.hi = std::stoul(gen["hi"]);
		ps.si = std::stoul(gen["si"]);
		ps.st = std::stoul(gen["st"]);
		ps.total = ps.us + ps.sy + ps.ni + ps.id + ps.wa + ps.hi + ps.si + ps.st;
		*/
	}
	getch();
	endwin();
	return (0);
}
