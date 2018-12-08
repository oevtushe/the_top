#include "Visual_ncs.hpp"

Visual_ncs::display_top_info(Top_info const &ti) const
{
	mvprintw(0, 0, "the_top - %s up %2ld:%ld, %2d user,  load average: %.2f, %.2f, %.2f\n",
			ti.curtime.c_str(),
			ti.uptime / 3600, // hours
			(ti.uptime % 3600) / 60, // minutes
			ti.num_of_users,
			ti.la_1,
			ti.la_5,
			ti.la_15);
}

Visual_ncs::display_tasks_info(Tasks_info const &ti) const
{
	mvprintw(1, 0, "Tasks: %3d total, %3d running, %3d sleeping, %3d stopped, %3d zombie\n",
			ti.total,
			ti.running,
			ti.sleeping,
			ti.stopped,
			ti.zombie);
}

Visual_ncs::display_cpu_info(Cpu_info const &ci) const
{
}

Visual_ncs::display_mem_info(Mem_info const &mi) const
{
	mvprintw(3, 0, "KiB Mem : %8.8lu total, %8.8lu free, %8.8lu used, %8.8lu buff/cache\n",
			mi.total,
			mi.free,
			mi.used,
			mi.buffers);
}

Visual_ncs::display_swap_info(Swap_info const &si) const
{
	mvprintw(4, 0, "KiB Swap: %8lu total, %8lu free, %8lu used. %8.8lu avail Mem\n",
			si.total,
			si.free,
			si.total - memi.free,
			si.available);
}

Visual_ncs::display_procs_info(Proc_info const &pi) const
{
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
	printw("%5d %-9.9s %2d %3d %7d %7d %7d %c %4.4s %4d %9d %-s\n",
			pi.pid,
			pi.user.c_str(),
			pi.priority,
			pi.nice,
			pi.vsize,
			pi.rss,
			pi.mem_shared,
			pi.state,
			"%CPU",
			0, //
			0, //
			pi.command.c_str());
}
