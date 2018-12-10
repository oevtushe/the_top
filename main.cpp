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

Cpu_usage	calc_cpu_usage(SysInfo::Cpuinfo const &prev, SysInfo::Cpuinfo const &cur)
{
	Cpu_usage	usage{};

	usage.us = cur.user - prev.user;
	usage.sy = cur.system - prev.system;
	usage.ni = cur.nice - prev.nice;
	usage.id = cur.idle - prev.idle;
	usage.wa = cur.iowait - prev.iowait;
	usage.hi = cur.irq - prev.irq;
	usage.si = cur.softirq - prev.softirq;
	usage.st = cur.steal - prev.steal;
	usage.total = usage.us + usage.sy + usage.ni + usage.id + usage.wa + usage.hi + usage.si + usage.st;
	usage.us = (static_cast<double>(usage.us) / usage.total) * 100.0;
	usage.sy = (static_cast<double>(usage.sy) / usage.total) * 100.0;
	usage.ni = (static_cast<double>(usage.ni) / usage.total) * 100.0;
	usage.id = (static_cast<double>(usage.id) / usage.total) * 100.0;
	usage.wa = (static_cast<double>(usage.wa) / usage.total) * 100.0;
	usage.hi = (static_cast<double>(usage.hi) / usage.total) * 100.0;
	usage.si = (static_cast<double>(usage.si) / usage.total) * 100.0;
	usage.st = (static_cast<double>(usage.st) / usage.total) * 100.0;
	return (usage);
}

void	setup(std::vector<SysInfo::Procinfo> &prev,
			std::vector<SysInfo::Procinfo> &cur, unsigned long int total)
{
	for (auto &p : cur)
	{
		auto it = std::find(prev.begin(), prev.end(), p);
		if (it != prev.end())
		{
			p.pcpu = (static_cast<double>(p.cpu - (*it).cpu) / total) * 100.0;
		}
	}
	std::sort(cur.begin(), cur.end(), [](SysInfo::Procinfo const &a, SysInfo::Procinfo const &b){
			if (a.pcpu > b.pcpu)
				return (1);
			else if (a.pcpu < b.pcpu)
				return (-1);
			else
				return (0);
	});
}

void	draw_screen(Visual_ncs const &ncs, Cpu_usage const &usage,
			std::vector<SysInfo::Procinfo> const &ccur, SysInfo const &si)
{
		ncs.display_top_info(si.get_curtime(), si.get_uptime(), si.get_num_of_users(), si.get_loadavg());
		ncs.display_tasks_info(si.get_tasks_count());
		ncs.display_cpu_info(usage);
		ncs.display_mem_info(si.get_mem_data());
		ncs.display_swap_info(si.get_mem_data());
		ncs.display_procs_info(ccur);
}

int		main(void)
{
	SysInfo							si;
	SysInfo							prev_si;
	Visual_ncs						ncs;
	SysInfo::Cpuinfo				prev{};
	SysInfo::Cpuinfo				cur;
	Cpu_usage						usage{};
	std::vector<SysInfo::Procinfo>	pprev;
	std::vector<SysInfo::Procinfo> 	ccur;
	std::vector<SysInfo::Procinfo> 	copy;

	while (true)
	{
		si.update();
		cur = si.get_cpu_data();
		ccur = si.get_procs_data();
		copy = ccur;
		usage = calc_cpu_usage(prev, cur);
		setup(pprev, ccur, usage.total);
		draw_screen(ncs, usage, ccur, si);
		refresh();
		sleep(3);
		erase();
		prev = cur;
		pprev = copy;
	}
	return (0);
}
