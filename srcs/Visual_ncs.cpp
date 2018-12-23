#include "Visual_ncs.hpp"
#include <sstream>
#include <unistd.h>

bool	operator==(IVisual::Procinfo const &a,
			IVisual::Procinfo const &b)
{
	return (a.pid == b.pid);
}

Visual_ncs::Visual_ncs() : _selected{}
{
	::initscr();
	::start_color();
	::keypad(stdscr, TRUE);
	::curs_set(0);
	::halfdelay(10); // tenths of second
	::init_color(33, 600, 600, 600); // grey color for table header
	::init_pair(1, COLOR_BLACK, 33);
	int	w = (COLS - 5) / 2;
	_meters = newwin(5, w, 1, 2);
	_text_info = newwin(5, w, 1, w + 3);
	_processes = newwin(LINES - 6, COLS, 6, 0);
	wborder(_meters, '|', '|', '-', '-', '+', '+', '+', '+');
	wborder(_text_info, '|', '|', '-', '-', '+', '+', '+', '+');
	wborder(_processes, '|', '|', '-', '-', '+', '+', '+', '+');
}

Visual_ncs::~Visual_ncs()
{
	::endwin();
	// del other windows
}

void	Visual_ncs::clean_screen() const
{
	::erase();
	::werase(_meters);
	::werase(_text_info);
	::werase(_processes);
}

void	Visual_ncs::refresh() const
{
	::refresh();
	wborder(_meters, '|', '|', '-', '-', '+', '+', '+', '+');
	wborder(_text_info, '|', '|', '-', '-', '+', '+', '+', '+');
	wborder(_processes, '|', '|', '-', '-', '+', '+', '+', '+');
	::wrefresh(_meters);
	::wrefresh(_text_info);
	::wrefresh(_processes);
}

int		Visual_ncs::read_ch()
{
	int		c{};

	switch((c = ::getch()))
	{
		case KEY_DOWN:
			++_selected;
			break ;
		case KEY_UP:
			if (_selected)
				--_selected;
			break ;
	}
	::flushinp();
	return (c);
}

void	Visual_ncs::display_cpu_bar(IVisual::Cpu_usage const &usage) const
{
	int		times{};
	int		x;
	int		y;

	getmaxyx(_meters, y, x);
	double	scale_factor{x / 100.0};
	mvwprintw(_meters, 1, 1, "CPU[");

	times = scale_factor * usage.ni + 0.5;
	for (int i = 0; i < times; ++i)
		wprintw(_meters, "L");
	times = scale_factor * usage.us + 0.5;
	for (int i = 0; i < times; ++i)
		wprintw(_meters, "X");
	times = scale_factor * (usage.sy + usage.wa + usage.hi) + 0.5;
	for (int i = 0; i < times; ++i)
		wprintw(_meters, "O");
	times = scale_factor * (usage.si + usage.st) + 0.5;
	for (int i = 0; i < times; ++i)
		wprintw(_meters, "$");

	double mytot = usage.ni + usage.us + usage.sy + usage.wa + usage.hi + usage.si + usage.st;
	mvwprintw(_meters, 1, x - 6, "%.1f%%]", mytot);
}

void	Visual_ncs::display_mem_bar(IVisual::Meminfo const &memi) const
{
	int		times{};
	unsigned long int used = memi.mem_used + memi.mem_buf + memi.mem_cache + memi.mem_slab;
	int		x;
	int		y;

	getmaxyx(_meters, y, x);
	mvwprintw(_meters, 2, 1, "Mem[");

	times = (x * used / memi.mem_total) + 0.5;
	for (int i = 0; i < times; ++i)
		wprintw(_meters, "U");
	times = (x * memi.mem_buf / memi.mem_total) + 0.5;
	for (int i = 0; i < times; ++i)
		wprintw(_meters, "B");
	times = (x * (memi.mem_cache + memi.mem_slab) / memi.mem_total) + 0.5;
	for (int i = 0; i < times; ++i)
		wprintw(_meters, "C");

	unsigned long int mem_used = memi.mem_used / 1024;
	unsigned long int mem_total = memi.mem_total / 1024;
	char unit_used = 'M';
	char unit_total = 'M';
	if (mem_used > 1000)
	{
		mem_used /= 1000;
		unit_used = 'G';
	}
	if (mem_total > 1000)
	{
		mem_total /= 1000;
		unit_total = 'G';
	}

	std::ostringstream	os;

	os.precision(2);
	os << std::fixed;
	os << mem_used << unit_used << "/";
	os << mem_total << unit_total;
	mvwprintw(_meters, 2, x - os.str().length() - 2, "%s]", os.str().c_str());
}

void	Visual_ncs::display_swap_bar(IVisual::Meminfo const &memi) const
{
	int		times{};
	int		x;
	int		y;

	getmaxyx(_meters, y, x);
	mvwprintw(_meters, 3, 1, "Swp[");

	times = (x * memi.swap_used / memi.swap_total) + 0.5;
	for (int i = 0; i < times; ++i)
		wprintw(_meters, "U");

	double swap_total = memi.swap_total / 1024.0;
	double swap_used = memi.swap_used / 1024.0;
	char unit_used = 'M';
	char unit_total = 'M';
	if (swap_used > 1024.0)
	{
		swap_used /= 1024.0;
		unit_used = 'G';
	}
	if (swap_total > 1024.0)
	{
		swap_total /= 1024.0;
		unit_total = 'G';
	}
	std::ostringstream	os;

	os.precision(2);
	os << std::fixed;
	os << swap_used << unit_used << "/";
	os << swap_total << unit_total;
	mvwprintw(_meters, 3, x - os.str().length() - 2, "%s]", os.str().c_str());
}

void	Visual_ncs::display_right_window(int threads, int tasks, int r,
			IVisual::Load_avg const &load_avg, long int uptime)
{
	mvwprintw(_text_info, 1, 1, "Tasks: %d, %d thr; %d running\n", tasks, threads, r);
	mvwprintw(_text_info, 2, 1, "Load average: %.2f %.2f %.2f\n", load_avg.la_1, load_avg.la_5, load_avg.la_15);
	mvwprintw(_text_info, 3, 1, "Uptime: %.2d:%.2d:%.2d\n", uptime / 3600, uptime % 3600 / 60, uptime % 60);
}

void	Visual_ncs::display_procs_info(std::vector<IVisual::Procinfo> const &pi)
{
	mvwprintw(_processes, 1, 1, "%5.5s %-9.9s %2.2s %3.3s %7.7s %6.6s "
				"%6.6s %1.1s %4.4s %4.4s %9.9s %s\n",
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
	mvwchgat(_processes, 1, 1, COLS - 2, A_NORMAL, 1, NULL);
	wmove(_processes, 1, 1);
	int		tck_sc = sysconf(_SC_CLK_TCK);
	int		sz = pi.size();
	int		max_show = LINES - 7; // to check
	if (_selected >= sz)
		_selected = sz - 1;
	for (auto const &proc : pi)
	for (int i = 0; i < pi.size(); ++i)
	{
		mvwprintw(_processes, i + 2, 1, "%5d %-9.9s %2.3s %3d %7d %6d %6d "
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
	mvwchgat(_processes, _selected + 2, 0, COLS - 1, A_NORMAL, 1, NULL);
	// if selected goes out of the screen , scroll processes
}
