#include <unistd.h>

#include <iostream>
#include <algorithm>
#include <memory>
#include <chrono>

#include "Visual_ncs.hpp"
#include "SysInfo.hpp"

static IVisual::Cpu_usage	calc_cpu_usage(IVisual::Cpuinfo const &prev,
		IVisual::Cpuinfo const &cur)
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
	usage.total = usage.us + usage.sy + usage.ni +
		usage.id + usage.wa + usage.hi + usage.si + usage.st;
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

static std::vector<IVisual::Procinfo>
get_procinfo(std::vector<SysInfo::Procinfo_raw> const &prev,
		std::vector<SysInfo::Procinfo_raw> const &cur, long int total)
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

		auto it = std::find(prev.begin(), prev.end(), proc);
		if (it != prev.end())
			pi.cpu = (static_cast<double>((proc.cpu - it->cpu) *
						sysconf(_SC_NPROCESSORS_ONLN)) / total) * 100.0;
		else
			pi.cpu = (static_cast<double>(proc.cpu) / total) * 100.0;
		procinfo.push_back(pi);
	}
	std::sort(procinfo.begin(), procinfo.end(),
			[](IVisual::Procinfo const &a, IVisual::Procinfo const &b){
			if (a.cpu >= 0.05 || b.cpu >= 0.05)
				return (a.cpu > b.cpu);
			else
				return (a.pid < b.pid);
	});
	return (procinfo);
}

/*
static void	draw_screen(std::shared_ptr<IVisual> const &ncs,
							IVisual::Cpu_usage const &usage,
							std::vector<IVisual::Procinfo> const &ccur,
							std::shared_ptr<ISys> const &si)
{
		ncs->display_cpu_bar(usage);
		ncs->display_mem_bar(si->get_mem_data());
		ncs->display_swap_bar(si->get_mem_data());
		int	running = std::count_if(ccur.begin(), ccur.end(), [](IVisual::Procinfo const &p) { return (p.state == 'R'); });
		ncs->display_right_window(si->get_thread_num(), ccur.size(),
				running, si->get_loadavg(), si->get_uptime());
		ncs->display_procs_info(ccur);
}
*/

void	fill_vdb(IVisual::Visual_db	&vdb,
					std::shared_ptr<ISys> const &si,
					IVisual::Cpu_usage const &usage,
					std::vector<IVisual::Procinfo> const &procinfo)
{
	vdb.usage = usage;
	vdb.procinfo = procinfo;
	//vdb.cpuinfo = cpuinfo;
	vdb.meminfo = si->get_mem_data();
	vdb.load_avg = si->get_loadavg();
	vdb.threads = si->get_thread_num();
	vdb.uptime = si->get_uptime();
}

/*
** There are two fundamental interfaces 'IVisual' for data
** displayers, 'ISys' for data loaders.
** Top works with this interfaces.
*/

int										main(void)
{
	int									wait_sec{3};
	std::shared_ptr<ISys>				si{new SysInfo{}};
	std::shared_ptr<IVisual>			ncs{new Visual_ncs{}};
	IVisual::Cpuinfo					prev{}; // for %Cpu(s) general
	IVisual::Cpuinfo					cur{};
	IVisual::Cpu_usage					usage{};
	std::vector<SysInfo::Procinfo_raw>	pprev{si->get_procs_data()}; // for %CPU per process
	std::vector<SysInfo::Procinfo_raw> 	ccur;
	std::vector<IVisual::Procinfo>		procinfo;
	IVisual::Visual_db					vdb{};
	std::future<void> fut{ncs->run_key_handler()};

	do
	{
		ncs->clear();
		si->update();
		cur = si->get_cpu_data();
		ccur = si->get_procs_data();
		usage = calc_cpu_usage(prev, cur);
		procinfo = get_procinfo(pprev, ccur, usage.total);
		fill_vdb(vdb, si, usage, procinfo);
		ncs->draw(vdb);
		prev = cur;
		pprev = ccur;
	} while (fut.wait_for(std::chrono::seconds(wait_sec)) == std::future_status::timeout);
	return (0);
}
