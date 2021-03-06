#include "Visual_top_ncs.hpp"

#include <unistd.h>

#include <ncurses.h>

bool	operator==(IVisual::Procinfo const &a,
			IVisual::Procinfo const &b)
{
	return (a.pid == b.pid);
}

Visual_top_ncs::Visual_top_ncs() : _offset{}
{
	::initscr();
	::start_color();
	::keypad(stdscr, TRUE);
	::curs_set(0);
	::halfdelay(30); // tenths of a second
	const int color_num{33};
	::init_color(color_num, 600, 600, 600); // grey color for table header
	::init_pair(1, COLOR_BLACK, color_num);
}

Visual_top_ncs::~Visual_top_ncs()
{
	::endwin();
}

void	Visual_top_ncs::clear()
{
	::erase();
}

void	Visual_top_ncs::draw(Visual_db const &db)
{
	_display_top_info(db.cur_time, db.uptime,
			db.num_of_users, db.load_avg);
	_display_tasks_info(db.tasks_count);
	_display_cpu_info(db.usage);
	_display_mem_info(db.meminfo);
	_display_swap_info(db.meminfo);
	_display_procs_info(db.procinfo);	
	::refresh();
}

bool	Visual_top_ncs::wait()
{

	switch(getch())
	{
		case KEY_DOWN:
			++_offset;
			break ;
		case KEY_UP:
			if (_offset)
				--_offset;
			break ;
		case 'q':
			return (false);
	}
	::flushinp();
	return (true);
}

void	Visual_top_ncs::_display_top_info(std::string const &cur_time,
			long int uptime, int nou, IVisual::Load_avg const &avg) const
{
	mvprintw(0, 0, "the_top - %s up %2ld:%.2ld, %2d users,  "
						"load average: %.2f, %.2f, %.2f\n",
			cur_time.c_str(),
			uptime / 3600, // hours
			(uptime % 3600) / 60, // minutes
			nou,
			avg.la_1,
			avg.la_5,
			avg.la_15);
}

void	Visual_top_ncs::_display_tasks_info(IVisual::Tasks_count const &tc) const
{
	mvprintw(1, 0, "Tasks: %3d total, %3d running, "
						"%3d sleeping, %3d stopped, %3d zombie\n",
			tc.total,
			tc.running,
			tc.sleeping,
			tc.stopped,
			tc.zombie);
}

void	Visual_top_ncs::_display_cpu_info(IVisual::Cpu_usage const &ci) const
{
	mvprintw(2, 0, "%%Cpu(s):%5.1f us,%5.1f sy,%5.1f ni,%5.1f id,"
			"%5.1f wa,%5.1f hi,%5.1f si,%5.1f st\n",
			ci.us,
			ci.sy,
			ci.ni,
			ci.id,
			ci.wa,
			ci.hi,
			ci.si,
			ci.st);
}

void	Visual_top_ncs::_display_mem_info(IVisual::Meminfo const &mi) const
{
	mvprintw(3, 0, "KiB Mem : %8lu total, "
					"%8lu free, %8lu used, %8lu buff/cache\n",
			mi.mem_total,
			mi.mem_free,
			mi.mem_used,
			mi.mem_buf + mi.mem_cache + mi.mem_sreclaimable);
}

void	Visual_top_ncs::_display_swap_info(IVisual::Meminfo const &mi) const
{
	mvprintw(4, 0, "KiB Swap: %8lu total, %8lu free, "
						"%8lu used. %8lu avail Mem\n",
			mi.swap_total,
			mi.swap_free,
			mi.swap_used,
			mi.available);
}

void	Visual_top_ncs::_display_procs_info(std::vector<IVisual::Procinfo> const &pi)
{
	mvprintw(6, 0, "%5.5s %-9.9s %2.2s %3.3s %7.7s %6.6s "
				"%6.6s %1.1s %4.4s %4.4s %9.9s %.7s\n",
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
	mvchgat(6, 0, -1, A_NORMAL, 1, nullptr);
	move(7, 0);
	const int	tck_sc = sysconf(_SC_CLK_TCK);
	int			sz = pi.size();
	const int	max_show = LINES - 7;
	if (_offset > sz)
		_offset = sz;
	if (sz - _offset > max_show)
		sz = max_show + _offset;
	for (int i = _offset; i < sz; ++i)
	{
		printw("%5d %-9.9s %2.3s %3d %7d %6d %6d "
					"%c %4.1f %4.1f %3.1lu:%.2lu.%.2lu %-s\n",
				pi[i].pid,
				pi[i].user.c_str(),
				pi[i].priority < -99 ? "rt" :
						std::to_string(pi[i].priority).c_str(),
				pi[i].nice,
				pi[i].vsize,
				pi[i].rss,
				pi[i].mem_shared,
				pi[i].state,
				pi[i].cpu,
				pi[i].memp,
				pi[i].timep / (60 * tck_sc), // minutes
				(pi[i].timep % 6000) / tck_sc, // seconds
				(pi[i].timep % 6000) % 100, // 1/100 second
				pi[i].command.c_str());
	}
}
