#include "Visual_ncs.hpp"
#include <sstream>
#include <unistd.h>

# define MY_RED 2
# define MY_BLUE 3
# define MY_GREEN 4
# define MY_YELLOW 5
# define MY_CYAN 6

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
	//::init_color(33, 600, 600, 600); // grey color for table header
	::init_pair(1, COLOR_BLACK, COLOR_GREEN);
	::init_pair(2, COLOR_RED, COLOR_BLACK);
	::init_pair(3, COLOR_BLUE, COLOR_BLACK);
	::init_pair(4, COLOR_GREEN, COLOR_BLACK);
	::init_pair(5, COLOR_YELLOW, COLOR_BLACK);
	::init_pair(6, COLOR_CYAN, COLOR_BLACK);
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

void	Visual_ncs::display_meter(int cp, int times)
{
	char	fill{'|'};

	wattron(_meters, COLOR_PAIR(cp));
	for (int i = 0; i < times; ++i)
	{
		if (winch(_meters) == ' ')
			wprintw(_meters, "%c", fill);
		else // paint digits in corresponding color
		{
			int	x, y;
			getyx(_meters, y, x);
			wchgat(_meters, 1, A_NORMAL, cp, nullptr);
			wmove(_meters, y, ++x);
		}
	}
	wattroff(_meters, COLOR_PAIR(cp));
}

static std::pair<double, char>	config_units(unsigned long int val)
{
	std::pair<double, char>	res{};

	if (val > 976563)
	{
		res.second = 'G';
		res.first = val / 976563.0;
	}
	else if (val > 1024)
	{
		res.second = 'M';
		res.first = val / 1024.0;
	}
	else
	{
		res.second = 'K';
		res.first = val;
	}
	return (res);
}

void	Visual_ncs::display_cpu_bar(IVisual::Cpu_usage const &usage)
{
	int		x;
	int		y;
	double	bar{};

	getmaxyx(_meters, y, x);
	mvwprintw(_meters, 1, 1, "CPU[");
	bar = (x - 7.0) / 100.0; // 7 is extra symbols in this window ('CPU[', left border, right border, ']')

	double mytot = usage.ni + usage.us + usage.sy + usage.wa + usage.hi + usage.si + usage.st;
	mvwprintw(_meters, 1, x - 8, "%5.1f%%]", mytot);

	wmove(_meters, 1, 5);
	int		times_ni = bar * usage.ni + 0.5;
	int		times_us = bar * usage.us + 0.5;
	int		times_kernel = bar * (usage.sy + usage.wa + usage.hi) + 0.5;
	int		times_virt = bar * (usage.si + usage.st) + 0.5;
	display_meter(MY_BLUE, times_ni);
	display_meter(MY_GREEN, times_us);
	display_meter(MY_RED, times_kernel);
	display_meter(MY_CYAN, times_virt);
}

void	Visual_ncs::display_mem_bar(IVisual::Meminfo const &memi)
{
	int		x;
	int		y;
	int		bar{};

	getmaxyx(_meters, y, x);
	bar = x - 7; // exclude 'CPU[', borders, ']'
	mvwprintw(_meters, 2, 1, "Mem[");

	unsigned long int mem_user = memi.mem_used +
		memi.mem_buf + memi.mem_cache + memi.mem_sreclaimable; // break into user,buffer,cache

	std::pair<double, char> used{config_units(mem_user)}; /////////// rename
	std::pair<double, char> total{config_units(memi.mem_total)};

	std::ostringstream	os;

	os.precision(2);
	os << std::fixed;
	os << used.first << used.second << "/";
	os << total.first << total.second;
	mvwprintw(_meters, 2, x - os.str().length() - 2, "%s]", os.str().c_str());

	wmove(_meters, 2, 5);

	double used_ratio = static_cast<double>(memi.mem_used) / memi.mem_total;
	double buf_ratio = static_cast<double>(memi.mem_buf) / memi.mem_total;
	double cache_ratio = static_cast<double>(memi.mem_cache + memi.mem_sreclaimable) / (memi.mem_total);
	int	times_used = bar * used_ratio + 0.5;
	int	times_buf = bar * buf_ratio + 0.5;
	int	times_cache = bar * cache_ratio + 0.5;

	display_meter(MY_GREEN, times_used);
	display_meter(MY_BLUE, times_buf);
	display_meter(MY_YELLOW, times_cache);
}

void	Visual_ncs::display_swap_bar(IVisual::Meminfo const &memi)
{
	int		times{};
	int		x;
	int		y;

	getmaxyx(_meters, y, x);
	mvwprintw(_meters, 3, 1, "Swp[");

	std::pair<double, char>	swap_total{config_units(memi.swap_total)};
	std::pair<double, char>	swap_used{config_units(memi.swap_used)};

	std::ostringstream	os;
	os.precision(2);
	os << std::fixed;
	os << swap_used.first << swap_used.second << "/";
	os << swap_total.first << swap_total.second;
	mvwprintw(_meters, 3, x - os.str().length() - 2, "%s]", os.str().c_str());

	wmove(_meters, 3, 5);
	double	used_ratio = static_cast<double>(memi.swap_used) / memi.swap_total;
	int		times_used = x * used_ratio + 0.5;
	display_meter(MY_RED, times_used);
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
