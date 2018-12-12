#include "Visual_ncs.hpp"
#include "SysInfo.hpp"
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <memory>

static IVisual::Cpu_usage				calc_cpu_usage(IVisual::Cpuinfo const &prev, IVisual::Cpuinfo const &cur)
{
	IVisual::Cpu_usage	usage{};

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

static std::vector<IVisual::Procinfo>	get_procinfo(std::vector<SysInfo::Procinfo_raw> const &prev,
		std::vector<SysInfo::Procinfo_raw> const &cur, unsigned long int total)
{
	std::vector<IVisual::Procinfo>	procinfo;

	for (auto const &proc : cur)
	{
		IVisual::Procinfo	pi{};

		pi.pid = proc.pid;
		pi.command = proc.command;
		pi.state = proc.state;
		pi.nice = proc.nice;
		pi.priority = proc.priority;
		pi.vsize = proc.vsize;
		pi.rss = proc.rss;
		pi.mem_shared = proc.mem_shared;
		pi.user = proc.user;
		pi.timep = proc.timep;
		pi.memp = proc.memp;

		// We need to bind 2 system snapshots (different time)
		// to calculate %CPU usage.
		// New snapshot may contain new processes, so we need to search
		// for those which exists in both and get difference in cpu.
		//
		// "Do not optimize till you have no need to" - Someone said

		auto it = std::find(prev.begin(), prev.end(), proc);
		if (it != prev.end())
			pi.cpu = (static_cast<double>(proc.cpu - (*it).cpu) / total) * 100.0;
		else
			pi.cpu = (static_cast<double>(proc.cpu) / total) * 100.0;
		procinfo.push_back(pi);
	}
	std::sort(procinfo.begin(), procinfo.end(), [](IVisual::Procinfo const &a, IVisual::Procinfo const &b){
			return (a.cpu > b.cpu);
	});
	return (procinfo);
}

static void								draw_screen(std::unique_ptr<IVisual> const &ncs,
											IVisual::Cpu_usage const &usage,
				std::vector<IVisual::Procinfo> const &ccur, std::unique_ptr<ISys> const &si)
{
		ncs->display_top_info(si->get_curtime(), si->get_uptime(),
				si->get_num_of_users(), si->get_loadavg());
		ncs->display_tasks_info(si->get_tasks_count());
		ncs->display_cpu_info(usage);
		ncs->display_mem_info(si->get_mem_data());
		ncs->display_swap_info(si->get_mem_data());
		ncs->display_procs_info(ccur);
}

/*
** There are two fundamental interfaces 'IVisual' for data
** displayers, 'ISys' for data loaders.
** Top works with this interfaces.
*/

int										main(void)
{
	std::unique_ptr<ISys>				si{new SysInfo{}};
	std::unique_ptr<IVisual>			ncs{new Visual_ncs{}};
	IVisual::Cpuinfo					prev{}; // for %Cpu(s) general
	IVisual::Cpuinfo					cur{};
	IVisual::Cpu_usage					usage{};
	std::vector<SysInfo::Procinfo_raw>	pprev{si->get_procs_data()}; // for %CPU per process
	std::vector<SysInfo::Procinfo_raw> 	ccur;
	std::vector<IVisual::Procinfo>		procinfo;

	do
	{
		ncs->clean_screen();
		si->update();
		cur = si->get_cpu_data();
		ccur = si->get_procs_data();
		usage = calc_cpu_usage(prev, cur);
		procinfo = get_procinfo(pprev, ccur, usage.total);
		draw_screen(ncs, usage, procinfo, si);
		ncs->refresh();
		prev = cur;
		pprev = ccur;
	} while ((ncs->read_ch() != 'q'));
	return (0);
}
