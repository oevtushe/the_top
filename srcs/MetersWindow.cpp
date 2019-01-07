#include "MetersWindow.hpp"

#include <string.h>

#include <sstream>
#include <iomanip>

# include <ncurses.h>

MetersWindow::MetersWindow(int nlines, int ncols, int begin_y, int begin_x) :
	Window(nlines, ncols, begin_y, begin_x)
{
	::keypad(_win, TRUE);
}

void	MetersWindow::draw(IVisual::Meminfo const &memi, IVisual::Cpu_usage const &usage)
{
	_display_cpu_bar(usage);
	_display_mem_bar(memi);
	_display_swap_bar(memi);
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

void	MetersWindow::_display_meter(int cp, int times)
{
	char	fill{'|'};

	wattron(_win, COLOR_PAIR(cp));
	for (int i = 0; i < times; ++i)
	{
		if (winch(_win) == ' ')
			wprintw(_win, "%c", fill);
		else // paint text in corresponding color
		{
			int	x, y;
			getyx(_win, y, x);
			wchgat(_win, 1, A_NORMAL, cp, nullptr);
			wmove(_win, y, ++x);
		}
	}
	wattroff(_win, COLOR_PAIR(cp));
}

void	MetersWindow::_display_cpu_bar(IVisual::Cpu_usage const &usage)
{
	int					x{};
	int					y{};
	auto				bar_lb{"CPU["}; // lb -> left border
	auto				bar_rb{"]"};

	getmaxyx(_win, y, x);
	y = 1; // line pos in cur _win window
	mvwprintw(_win, y, 1, bar_lb);
	const double bar = (x - strlen(bar_lb) - strlen(bar_rb) - 2) / 100.0; // -2 excludes borders
	const double mytot = usage.ni + usage.us + usage.sy +
					usage.wa + usage.hi + usage.si + usage.st;

	std::ostringstream	os;
	os.precision(1);
	os << std::fixed;
	os << mytot << "%" << bar_rb;
	mvwprintw(_win, y, x - os.str().length() - 1,
					"%s", os.str().c_str()); // -1 excludes border
	wmove(_win, y, strlen(bar_lb) + 1);

	const int times_ni = bar * usage.ni + 0.5;
	const int times_us = bar * usage.us + 0.5;
	const int times_kernel = bar * (usage.sy + usage.wa + usage.hi) + 0.5;
	const int times_virt = bar * (usage.si + usage.st) + 0.5;
	_display_meter(MY_BLUE, times_ni);
	_display_meter(MY_GREEN, times_us);
	_display_meter(MY_RED, times_kernel);
	_display_meter(MY_CYAN, times_virt);
}

void	MetersWindow::_display_mem_bar(IVisual::Meminfo const &memi)
{
	int		x;
	int		y;
	auto	bar_lb{"Mem["};
	auto	bar_rb{"]"};

	getmaxyx(_win, y, x);
	y = 2; // line pos in cur _win window
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
	mvwprintw(_win, y, 1, bar_lb);
	mvwprintw(_win, y, x - os.str().length() - 1,
				"%s", os.str().c_str()); // -1 excludes right border

	const double used_ratio = static_cast<double>(memi.mem_used) / memi.mem_total;
	const double buf_ratio = static_cast<double>(memi.mem_buf) / memi.mem_total;
	const double cache_ratio = static_cast<double>(memi.mem_cache + memi.mem_sreclaimable) / (memi.mem_total);
	const int	 times_used = bar_size * used_ratio + 0.5;
	const int	 times_buf = bar_size * buf_ratio + 0.5;
	const int	 times_cache = bar_size * cache_ratio + 0.5;

	wmove(_win, y, strlen(bar_lb) + 1);
	_display_meter(MY_GREEN, times_used);
	_display_meter(MY_BLUE, times_buf);
	_display_meter(MY_YELLOW, times_cache);
}

void	MetersWindow::_display_swap_bar(IVisual::Meminfo const &memi)
{
	int		x;
	int		y;
	auto	bar_lb{"Swp["};
	auto	bar_rb{"]"};

	getmaxyx(_win, y, x);
	y = 3; // line pos in cur _win window
	mvwprintw(_win, y, 1, bar_lb);

	std::pair<double, char>	swap_total{config_units(memi.swap_total)};
	std::pair<double, char>	swap_used{config_units(memi.swap_used)};

	std::ostringstream	os;
	const int total_prec = swap_total.second == 'G' ? 2 : 0;
	const int used_prec = swap_used.second == 'G' ? 2 : 0;
	os << std::fixed;
	os << std::setprecision(used_prec) << swap_used.first << swap_used.second << "/";
	os << std::setprecision(total_prec) << swap_total.first << swap_total.second << bar_rb;
	mvwprintw(_win, y, x - os.str().length() - 1, "%s", os.str().c_str());

	wmove(_win, y, strlen(bar_lb) + 1);
	const double	used_ratio = static_cast<double>(memi.swap_used) / memi.swap_total;
	const int		times_used = x * used_ratio + 0.5;
	_display_meter(MY_RED, times_used);
}
