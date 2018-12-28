#include "Visual_ncs.hpp"
#include <sstream>
#include <unistd.h>
#include <string.h>

bool	operator==(IVisual::Procinfo const &a,
			IVisual::Procinfo const &b)
{
	return (a.pid == b.pid);
}

Visual_ncs::Visual_ncs() : _selected{}
{
	::initscr();
	::start_color();
	::curs_set(0);
	::init_pair(MY_HEADER, COLOR_BLACK, COLOR_GREEN);
	::init_pair(MY_RED, COLOR_RED, COLOR_BLACK);
	::init_pair(MY_BLUE, COLOR_BLUE, COLOR_BLACK);
	::init_pair(MY_GREEN, COLOR_GREEN, COLOR_BLACK);
	::init_pair(MY_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	::init_pair(MY_CYAN, COLOR_CYAN, COLOR_BLACK);
	::init_pair(MY_LINE, COLOR_BLACK, COLOR_CYAN);
	_init_windows();
}

void	Visual_ncs::_init_windows()
{
	// add space between windows if can't divide evenly
	const int space_between = COLS % 2 ? 2 : 1;
	const int align_left_right = 2;

	const int meters_height = 5; // additional 2 is borders
	const int meters_width = (COLS - align_left_right * 2 - space_between) / 2; // always divides evenly
																				// thanks to space_between
	const int meters_start_x = 2;
	const int meters_start_y = 1;
	_meters = newwin(meters_height, meters_width, meters_start_y, meters_start_x);

	const int text_info_height = meters_height;
	const int text_info_width = meters_width;
	const int text_info_start_x = meters_width + meters_start_x + space_between;
	const int text_info_start_y = meters_start_y;
	_text_info = newwin(text_info_height, text_info_width, text_info_start_y, text_info_start_x);

	const int processes_height = LINES - meters_height - meters_start_y;
	const int processes_width = COLS;
	const int proc_start_y = meters_height + meters_start_y;
	const int proc_start_x = 0;
	_processes = newwin(processes_height, processes_width, proc_start_y, proc_start_x);

	_vp_start = 0;
	int x,y;
	getmaxyx(_processes, y, x);
	_vp_end = y - 3; // -3 is 2 borders, 1 header line
	_selected = _vp_start;
	::keypad(_processes, TRUE);
}

void	Visual_ncs::_del_wins()
{
	// clean buffers and screen before
	clean_screen();
	refresh();
	delwin(_meters);
	delwin(_text_info);
	delwin(_processes);
}

Visual_ncs::~Visual_ncs()
{
	_del_wins();
	::endwin();
}

void	Visual_ncs::clean_screen() const
{
	::werase(_meters);
	::werase(_text_info);
	::werase(_processes);
}

void	Visual_ncs::refresh() const
{
	::wrefresh(_meters);
	::wrefresh(_text_info);
	::wrefresh(_processes);
}

std::future<void>	Visual_ncs::run_key_handler()
{
	return (std::async(std::launch::async, &Visual_ncs::_key_handler, this));
}

void	Visual_ncs::_key_handler()
{
	while (int c = wgetch(_processes))
	{
		switch (c)
		{
			case 'q': return ;
			case KEY_UP:
			{
				_handle_up_vp_border();
				break ;
			}
			case KEY_DOWN:
			{
				_handle_down_vp_border();
			  	break ;
			}
			case KEY_RESIZE:
			{
				_del_wins();
				::refresh();
				_init_windows();
				break ;
			}
		}
		werase(_processes);
		display_procs_info(_procinfo);
		wrefresh(_processes);
	}
}

void	Visual_ncs::_handle_up_vp_border()
{
	if (_selected)
		--_selected;
	else
	{
		if (_vp_start)
		{
			--_vp_start;
			--_vp_end;
		}
	}
}

void	Visual_ncs::_handle_down_vp_border()
{
	if (_selected + _vp_start + 1 >= _vp_end)
	{
		if (_vp_end < _procinfo.size())
		{
			++_vp_end;
			++_vp_start;
		}
	}
	else if (_selected  + 1 < _procinfo.size())
		++_selected;
}

void	Visual_ncs::display_meter(int cp, int times)
{
	char	fill{'|'};

	wattron(_meters, COLOR_PAIR(cp));
	for (int i = 0; i < times; ++i)
	{
		if (winch(_meters) == ' ')
			wprintw(_meters, "%c", fill);
		else // paint text in corresponding color
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
	constexpr int			kib_in_gib{1048576};
	constexpr int 			kib_in_mib{1024};
	std::pair<double, char>	res{};

	if (val > kib_in_gib)
	{
		res.second = 'G';
		res.first = static_cast<double>(val) / kib_in_gib;
	}
	else if (val > kib_in_mib)
	{
		res.second = 'M';
		res.first = static_cast<double>(val) / kib_in_mib;
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
	int					x{};
	int					y{};
	auto				bar_lb{"CPU["}; // lb -> left border
	auto				bar_rb{"]"};

	getmaxyx(_meters, y, x);
	y = 1; // line pos in cur _meters window
	mvwprintw(_meters, y, 1, bar_lb);
	const int bar_size = (x - strlen(bar_lb) - strlen(bar_rb) - 2) / 100.0; // -2 excludes borders

	const double mytot = usage.ni + usage.us + usage.sy +
					usage.wa + usage.hi + usage.si + usage.st;

	std::ostringstream	os;
	os.precision(1);
	os << std::fixed;
	os << mytot << "%" << bar_rb;
	mvwprintw(_meters, y, x - os.str().length() - 1,
					"%s", os.str().c_str()); // -1 excludes borders

	wmove(_meters, y, strlen(bar_lb) + 1);

	const int times_ni = bar_size * usage.ni + 0.5;
	const int times_us = bar_size * usage.us + 0.5;
	const int times_kernel = bar_size * (usage.sy + usage.wa + usage.hi) + 0.5;
	const int times_virt = bar_size * (usage.si + usage.st) + 0.5;
	display_meter(MY_BLUE, times_ni);
	display_meter(MY_GREEN, times_us);
	display_meter(MY_RED, times_kernel);
	display_meter(MY_CYAN, times_virt);
}

void	Visual_ncs::display_mem_bar(IVisual::Meminfo const &memi)
{
	int		x;
	int		y;
	auto	bar_lb{"Mem["};
	auto	bar_rb{"]"};

	getmaxyx(_meters, y, x);
	y = 2; // line pos in cur _meters window
	const int bar_size = x - strlen(bar_lb) - strlen(bar_rb) - 2; // -2 exludes borders

	// count buffers and cache as used memory also
	// for text output
	const unsigned long int mem_user = memi.mem_used +
		memi.mem_buf + memi.mem_cache + memi.mem_sreclaimable;

	std::pair<double, char> user{config_units(mem_user)};
	std::pair<double, char> total{config_units(memi.mem_total)};

	std::ostringstream	os;
	os.precision(2);
	os << std::fixed;
	os << user.first << user.second << "/";
	os << total.first << total.second << bar_rb;

	// display borders for bar
	mvwprintw(_meters, y, 1, bar_lb);
	mvwprintw(_meters, y, x - os.str().length() - 1,
				"%s", os.str().c_str()); // -1 excludes right border

	const double used_ratio = static_cast<double>(memi.mem_used) / memi.mem_total;
	const double buf_ratio = static_cast<double>(memi.mem_buf) / memi.mem_total;
	const double cache_ratio = static_cast<double>(memi.mem_cache + memi.mem_sreclaimable) / (memi.mem_total);
	const int	 times_used = bar_size * used_ratio + 0.5;
	const int	 times_buf = bar_size * buf_ratio + 0.5;
	const int	 times_cache = bar_size * cache_ratio + 0.5;

	wmove(_meters, y, strlen(bar_lb) + 1);
	display_meter(MY_GREEN, times_used);
	display_meter(MY_BLUE, times_buf);
	display_meter(MY_YELLOW, times_cache);
}

void	Visual_ncs::display_swap_bar(IVisual::Meminfo const &memi)
{
	int		x;
	int		y;
	auto	bar_lb{"Swp["};
	auto	bar_rb{"]"};

	getmaxyx(_meters, y, x);
	y = 3; // line pos in cur _meters window
	mvwprintw(_meters, y, 1, bar_lb);

	std::pair<double, char>	swap_total{config_units(memi.swap_total)};
	std::pair<double, char>	swap_used{config_units(memi.swap_used)};

	std::ostringstream	os;
	os.precision(2);
	os << std::fixed;
	os << swap_used.first << swap_used.second << "/";
	os << swap_total.first << swap_total.second << bar_rb;
	mvwprintw(_meters, y, x - os.str().length() - 1, "%s", os.str().c_str());

	wmove(_meters, y, strlen(bar_lb) + 1);
	const double	used_ratio = static_cast<double>(memi.swap_used) / memi.swap_total;
	const int		times_used = x * used_ratio + 0.5;
	display_meter(MY_RED, times_used);
}

void	Visual_ncs::display_right_window(int threads, int tasks, int r,
			IVisual::Load_avg const &load_avg, long int uptime)
{
	mvwprintw(_text_info, 1, 1, "Tasks: %d, %d thr; %d running\n", tasks, threads, r);
	mvwprintw(_text_info, 2, 1, "Load average: %.2f %.2f %.2f\n", load_avg.la_1, load_avg.la_5, load_avg.la_15);
	mvwprintw(_text_info, 3, 1, "Uptime: %.2d:%.2d:%.2d\n", uptime / 3600, uptime % 3600 / 60, uptime % 60);
}

void	Visual_ncs::_display_header()
{
	mvwprintw(_processes, 1, 1, "%5.5s %-9.9s %2.2s %3.3s %7.7s %6.6s "
				"%6.6s %1.1s %4.4s %4.4s %9.9s %s",
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
	mvwchgat(_processes, 1, 1, COLS - 2, A_NORMAL, MY_HEADER, NULL);
	wmove(_processes, 1, 1);
}

void	Visual_ncs::display_procs_info(std::vector<IVisual::Procinfo> const &pi)
{
	const int tck_sc = sysconf(_SC_CLK_TCK);
	const int times = _vp_end > pi.size() ? pi.size() : _vp_end;

	_display_header();
	for (int i = _vp_start, j = 0; i < times; ++i, ++j)
	{
		mvwprintw(_processes, j + 2, 1, "%5d %-9.9s %2.3s %3d %7d %6d %6d "
					"%c %4.1f %4.1f %3.1lu:%.2lu.%.2lu %-s",
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
	_procinfo = pi; // steal data for key_handler
	_display_cursor();
}

void	Visual_ncs::_display_cursor()
{
	// move 'selected' back if prev
	// position is too low
	if (_selected >= _procinfo.size())
		_selected = _procinfo.size() - 1;
	mvwchgat(_processes, _selected + 2, 1, COLS - 2, A_NORMAL, MY_LINE, NULL);
}
