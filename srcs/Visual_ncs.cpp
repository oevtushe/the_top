#include "Visual_ncs.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unistd.h>
#include <string.h>
#include <signal.h>

bool	operator==(IVisual::Procinfo const &a,
			IVisual::Procinfo const &b)
{
	return (a.pid == b.pid);
}

Visual_ncs::Visual_ncs() : _pw{}
{
	::initscr();
	::start_color();
	::noecho();
	::cbreak();
	::curs_set(0);
	::init_pair(MY_HEADER, COLOR_BLACK, COLOR_GREEN);
	::init_pair(MY_RED, COLOR_RED, COLOR_BLACK);
	::init_pair(MY_BLUE, COLOR_BLUE, COLOR_BLACK);
	::init_pair(MY_GREEN, COLOR_GREEN, COLOR_BLACK);
	::init_pair(MY_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	::init_pair(MY_CYAN, COLOR_CYAN, COLOR_BLACK);
	::init_pair(MY_LINE, COLOR_BLACK, COLOR_CYAN);
	::init_pair(MY_ULINE, COLOR_BLACK, COLOR_WHITE);
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

	const int text_info_start_x = meters_width + meters_start_x + space_between;
	_text_info = newwin(meters_height, meters_width, meters_start_y, text_info_start_x);

	const int processes_height = LINES - meters_height - meters_start_y;
	const int processes_width = COLS;
	const int proc_start_y = meters_height + meters_start_y;
	const int proc_start_x = 0;
	_pw = new ProcessWindow(processes_height, processes_width, proc_start_y, proc_start_x);

	const int sig_width = 16;
	_sw = new SignalsWindow(processes_height, sig_width, proc_start_y, proc_start_x);

	::keypad(_meters, TRUE);
}

void	Visual_ncs::_del_wins()
{
	// clean buffers and screen before
	clean_screen();
	refresh();
	delwin(_meters);
	delwin(_text_info);
	delete _pw;
	delete _sw;
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
	_pw->erase();
	if (_is_signals_visible)
		_sw->erase();
}

void	Visual_ncs::refresh() const // make private
{
	::wrefresh(_meters);
	::wrefresh(_text_info);
	_pw->refresh();
	if (_is_signals_visible)
		_sw->refresh();
}

void	Visual_ncs::draw_screen(Visual_db const &db)
{
		display_cpu_bar(db.usage);
		display_mem_bar(db.meminfo);
		display_swap_bar(db.meminfo);
		int	running = std::count_if(db.procinfo.begin(), db.procinfo.end(), [](IVisual::Procinfo const &p) { return (p.state == 'R'); });
		display_right_window(db.threads, db.procinfo.size(), // make private
				running, db.load_avg, db.uptime);
		_pw->set_data(db.procinfo);
		_pw->draw();
		if (_is_signals_visible)
			_sw->draw();
}

std::future<void>	Visual_ncs::run_key_handler()
{
	return (std::async(std::launch::async, &Visual_ncs::_key_handler, this));
}

void	Visual_ncs::_resize()
{
	_del_wins();
	::refresh();
	_init_windows();
}

void	Visual_ncs::_open_signals_window()
{
	std::pair<int,int> proc_size{_pw->get_size()};
	std::pair<int,int> proc_pos{_pw->get_pos()};
	std::pair<int,int> sig_size{_sw->get_size()};
	std::pair<int,int> sig_pos{_sw->get_pos()};

	// put process window right to signal window
	_pw->resize(proc_size.first, COLS - sig_size.second,
			proc_pos.first, proc_pos.second + sig_size.second);
	_sw->draw();
	_sw->refresh();
	_pw->draw();
	_pw->refresh();
}

void	Visual_ncs::_key_handler()
{
	while (true)
	{
		switch (int c = wgetch(_meters))
		{
			case 'k':
				if (!_is_signals_visible)
				{
					_pw->freeze();
					_open_signals_window();
					_is_signals_visible = true;
				}
				break ;
			case 'q':
				if (_is_signals_visible)
				{
					_sw->clear();
					std::pair<int,int> size{_pw->get_size()};
					std::pair<int,int> pos{_pw->get_pos()};
					_pw->resize(size.first, COLS, pos.first, 0);
					_pw->unfreeze();
					_pw->draw();
					_pw->refresh();
					_is_signals_visible = false;
				}
				else
					return ; // exit
				break ;
			case KEY_RESIZE:
				_resize();
				_is_signals_visible = false;
				break ;
			default:
				ungetch(c);
				if (!_is_signals_visible)
					_pw->handle_input();
				else
					_sw->handle_input(_pw->get_selected_pid());
		}
	}
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
	const double bar = (x - strlen(bar_lb) - strlen(bar_rb) - 2) / 100.0; // -2 excludes borders
	const double mytot = usage.ni + usage.us + usage.sy +
					usage.wa + usage.hi + usage.si + usage.st;

	std::ostringstream	os;
	os.precision(1);
	os << std::fixed;
	os << mytot << "%" << bar_rb;
	mvwprintw(_meters, y, x - os.str().length() - 1,
					"%s", os.str().c_str()); // -1 excludes border
	wmove(_meters, y, strlen(bar_lb) + 1);

	const int times_ni = bar * usage.ni + 0.5;
	const int times_us = bar * usage.us + 0.5;
	const int times_kernel = bar * (usage.sy + usage.wa + usage.hi) + 0.5;
	const int times_virt = bar * (usage.si + usage.st) + 0.5;
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

	const int user_prec = user.second == 'G' ? 2 : 0;
	const int total_prec = total.second == 'G' ? 2 : 0;
	std::ostringstream	os;
	os << std::fixed;
	os << std::setprecision(user_prec) << user.first << user.second << "/";
	os << std::setprecision(total_prec) << total.first << total.second << bar_rb;

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
	const int total_prec = swap_total.second == 'G' ? 2 : 0;
	const int used_prec = swap_used.second == 'G' ? 2 : 0;
	os << std::fixed;
	os << std::setprecision(used_prec) << swap_used.first << swap_used.second << "/";
	os << std::setprecision(total_prec) << swap_total.first << swap_total.second << bar_rb;
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
