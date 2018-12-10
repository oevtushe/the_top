#include "Visual_ncs.hpp"
#include "SysInfo.hpp"
#include "The_top.hpp"
#include <iostream>
#include <unistd.h>

#include <ncurses.h>
static void	draw_screen(Visual_ncs const &ncs, The_top::Cpu_usage const &usage,
				std::vector<The_top::Procinfo> const &ccur, SysInfo const &si)
{
		ncs.display_top_info(si.get_curtime(), si.get_uptime(),
				si.get_num_of_users(), si.get_loadavg());
		ncs.display_tasks_info(si.get_tasks_count());
		ncs.display_cpu_info(usage);
		ncs.display_mem_info(si.get_mem_data());
		ncs.display_swap_info(si.get_mem_data());
		ncs.display_procs_info(ccur);
}

int		main(void)
{
	SysInfo								si;
	Visual_ncs							ncs;
	SysInfo::Cpuinfo					prev{}; // for %Cpu(s) general
	SysInfo::Cpuinfo					cur;
	The_top::Cpu_usage					usage{};
	std::vector<SysInfo::Procinfo_raw>	pprev; // for %CPU per process
	std::vector<SysInfo::Procinfo_raw> 	ccur;
	std::vector<The_top::Procinfo>		procinfo;

	do
	{
		ncs.clean_screen();
		si.update();
		cur = si.get_cpu_data();
		ccur = si.get_procs_data();
		usage = The_top::calc_cpu_usage(prev, cur);
		procinfo = The_top::get_procinfo(pprev, ccur, usage.total);
		draw_screen(ncs, usage, procinfo, si);
		ncs.refresh();
		prev = cur;
		pprev = ccur;
	} while ((ncs.read_ch() != 033));
	return (0);
}
