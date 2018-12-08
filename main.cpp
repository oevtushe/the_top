#include "Visual_ncs.hpp"
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

void	init_top_info(The_top::Top_info &ti, SysInfo const &si)
{
	ti.cur_time = si.get_curtime();
	ti.uptime = si.get_uptime();
	ti.num_of_users = si.get_num_of_users();
	std::array<double, 3> avg = si.get_loadavg();
	ti.la_1 = avg[0];
	ti.la_5 = avg[1];
	ti.la_15 = avg[2];
}

void	init_tasks_info(The_top::Tasks_info &ti, SysInfo const &si)
{
	std::vector<SysInfo::Procinfo> const &proc = si.get_procs_data();

	ti.total = proc.size();
	ti.running = std::count_if(proc.begin(), proc.end(), [](SysInfo::Procinfo const &t){ return (t.state == 'R');});
	ti.sleeping = std::count_if(proc.begin(), proc.end(), [](SysInfo::Procinfo const &t){ return (t.state == 'S');});
	ti.stopped = std::count_if(proc.begin(), proc.end(), [](SysInfo::Procinfo const &t){ return (t.state == 'T');});
	ti.zombie = std::count_if(proc.begin(), proc.end(), [](SysInfo::Procinfo const &t){ return (t.state == 'Z');});
}

void	init_cpu_info()
{
}

void	init_mem_info(The_top::Mem_info &ti, SysInfo const &si)
{
	SysInfo::Meminfo const &mi = si.get_mem_data();

	ti.total = mi.mem_total;
	ti.free = mi.mem_free;
	ti.bc = mi.buffer + mi.cache + mi.sreclaimable;
	ti.used = mi.mem_total - mi.mem_free - ti.bc;
}

void	init_swap_info(The_top::Swap_info &ti, SysInfo const &si)
{
	SysInfo::Meminfo const &mi = si.get_mem_data();

	ti.total = mi.swap_total;
	ti.free = mi.swap_free;
	ti.used = mi.swap_total - mi.swap_free;
	ti.avail = mi.available;
}

void	init_procs_info(std::vector<The_top::Proc_info> &pi, SysInfo const &si)
{
	std::vector<SysInfo::Procinfo> const &procs = si.get_procs_data();

	std::transform(procs.begin(), procs.end(), pi.begin(), pi.begin(), [](SysInfo::Procinfo const &pi_old, The_top::Proc_info &pi_tmp){
			The_top::Proc_info pi_new;

			pi_new.pid = pi_old.pid;
			pi_new.user = pi_old.user;
			pi_new.priority = pi_old.priority;
			pi_new.nice = pi_old.nice;
			pi_new.vsize = pi_old.vsize;
			pi_new.res = pi_old.rss;
			pi_new.shared = pi_old.mem_shared;
			pi_new.state = pi_old.state;
			//pi_new.cpu = pi_old.cpu; //
			//pi_new.mem = pi_old.mem;
			pi_new.timep = (pi_old.utime + pi_old.stime) / sysconf(_SC_CLK_TCK);
			pi_new.command = pi_old.command;
			return (pi_new);
	});
}

void	display_gen_info(SysInfo const &si)
{
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
}

void	display_proc_info(std::vector<SysInfo::Procinfo> const &procs)
{
	move(6, 0);
	for (auto &proc : procs)
	{
		///////////////////////////////////////// can be replaced
		/*
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
				*/
		/////////////////////////////////////////
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
	Visual_ncs	ncs;
	struct prev_state ps{};
	struct prev_state ps_dif{};

	The_top::Top_info	top_info;
	The_top::Tasks_info	tasks_info;
	The_top::Cpu_info	cpu_info;
	The_top::Mem_info	mem_info;
	The_top::Swap_info	swap_info;
	std::vector<The_top::Proc_info>	proc_info;

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
		The_top::init_top_info(top_info, si);
		The_top::init_tasks_info(tasks_info, si);
		//ncs.display_cpu_info(si);
		The_top::init_mem_info(mem_info, si);
		The_top::init_swap_info(swap_info, si);
		//ncs.display_swap_info(proc_info, si);
		//ncs.display_proc_info(si);
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
