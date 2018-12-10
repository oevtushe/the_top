#include <algorithm>
#include "The_top.hpp"

The_top::Cpu_usage	The_top::calc_cpu_usage(SysInfo::Cpuinfo const &prev, SysInfo::Cpuinfo const &cur)
{
	The_top::Cpu_usage	usage{};

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

std::vector<The_top::Procinfo>	The_top::get_procinfo(std::vector<SysInfo::Procinfo_raw> const &prev,
		std::vector<SysInfo::Procinfo_raw> const &cur, unsigned long int total)
{
	std::vector<The_top::Procinfo>	procinfo;

	for (auto const &proc : cur)
	{
		The_top::Procinfo	pi{};

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
	std::sort(procinfo.begin(), procinfo.end(), [](The_top::Procinfo const &a, The_top::Procinfo const &b){
			if (a.cpu > b.cpu)
				return (1);
			else if (a.cpu < b.cpu)
				return (-1);
			else
				return (0);
	});
	return (procinfo);
}

bool		operator==(The_top::Procinfo const &a, The_top::Procinfo const &b)
{
	return (a.pid == b.pid);
}
