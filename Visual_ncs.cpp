#include <ncurses.h>
#include <unistd.h>
#include "Visual_ncs.hpp"

Visual_ncs::Visual_ncs()
{
	::initscr();
	::start_color();
	::curs_set(0);
	::nodelay(stdscr, 1);
	::init_color(33, 600, 600, 600);
	::init_pair(1, COLOR_BLACK, 33);
}

Visual_ncs::~Visual_ncs()
{
	::endwin();
}

void	Visual_ncs::clean_screen() const
{
	::erase();
}

void	Visual_ncs::refresh() const
{
	::refresh();
}

void	Visual_ncs::display_top_info(std::string const &cur_time, long int uptime, int nou, SysInfo::Load_avg const &avg) const
{
	mvprintw(0, 0, "the_top - %s up %2ld:%ld, %2d user,  load average: %.2f, %.2f, %.2f\n",
			cur_time.c_str(),
			uptime / 3600, // hours
			(uptime % 3600) / 60, // minutes
			nou,
			avg.la_1,
			avg.la_5,
			avg.la_15);
}

void	Visual_ncs::display_tasks_info(SysInfo::Tasks_count const &tc) const
{
	mvprintw(1, 0, "Tasks: %3d total, %3d running, %3d sleeping, %3d stopped, %3d zombie\n",
			tc.total,
			tc.running,
			tc.sleeping,
			tc.stopped,
			tc.zombie);
}

void	Visual_ncs::display_cpu_info(The_top::Cpu_usage const &ci) const
{
	mvprintw(2, 0, "%%Cpu(s):%5.1f us,%5.1f sy,%5.1f ni,%5.1f id,%5.1f wa,%5.1f hi,%5.1f si,%5.1f st\n",
			ci.us,
			ci.sy,
			ci.ni,
			ci.id,
			ci.wa,
			ci.hi,
			ci.si,
			ci.st);
}

void	Visual_ncs::display_mem_info(SysInfo::Meminfo const &mi) const
{
	mvprintw(3, 0, "KiB Mem : %8lu total, %8lu free, %8lu used, %8lu buff/cache\n",
			mi.mem_total,
			mi.mem_free,
			mi.mem_used,
			mi.bc);
}

void	Visual_ncs::display_swap_info(SysInfo::Meminfo const &mi) const
{
	mvprintw(4, 0, "KiB Swap: %8lu total, %8lu free, %8lu used. %8lu avail Mem\n",
			mi.swap_total,
			mi.swap_free,
			mi.swap_used,
			mi.available);
}

void	Visual_ncs::display_procs_info(std::vector<The_top::Procinfo> const &pi) const
{
	mvprintw(6, 0, "%5.5s %-9.9s %2.2s %3.3s %7.7s %6.6s %6.6s %1.1s %4.4s %4.4s %9.9s %.7s\n",
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
	mvchgat(6, 0, -1, A_NORMAL, 1, NULL);
	move(7, 0);
	int		tck_sc = sysconf(_SC_CLK_TCK);
	for (auto const &proc : pi)
	{
		printw("%5d %-9.9s %2d %3d %7d %6d %6d %c %4.1f %4.1f %3.1lu:%.2lu.%.2lu %-s\n",
				proc.pid,
				proc.user.c_str(),
				proc.priority,
				proc.nice,
				proc.vsize,
				proc.rss,
				proc.mem_shared,
				proc.state,
				proc.cpu,
				proc.memp,
				proc.timep / (60 * tck_sc), // minutes
				(proc.timep % 6000) / tck_sc, // seconds
				(proc.timep % 6000) % 100, // 1/100 second
				proc.command.c_str());
	}
}
