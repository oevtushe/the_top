#include "ProcessWindow.hpp"
#include <algorithm>
#include <unistd.h>

ProcessWindow::ProcessWindow(int nlines, int ncols, int begin_y, int begin_x)
{
	_processes = newwin(nlines, ncols, begin_y, begin_x);
}

ProcessWindow::~ProcessWindow()
{
	werase(_processes);
	wrefresh(_processes);
	delwin(_processes);
}

void	ProcessWindow::draw(std::vector<IVisual::Procinfo> const &pi)
{
	_display_header();
	_display_procs_info(pi);
	_procinfo = pi;
}

void	ProcessWindow::erase()
{
	werase(_processes);
}

void	ProcessWindow::refresh()
{
	wrefresh(_processes);
}

void	ProcessWindow::clear()
{
	werase(_processes);
	wrefresh(_processes);
}

void	ProcessWindow::_display_header()
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
	int x, y;
	getmaxyx(_processes, y, x);
	mvwchgat(_processes, 1, 1, x - 2, A_NORMAL, MY_HEADER, NULL);
	wmove(_processes, 1, 1);
}

void	ProcessWindow::_display_procs_info(std::vector<IVisual::Procinfo> const &pi)
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
	//_procinfo = pi; // steal data for key_handler
}

void	ProcessWindow::_display_cursor()
{
	int x, y;

	getmaxyx(_processes, y, x);
	if (_selected + _vp_start >= _procinfo.size())
		_selected = 0;
	//if (!_is_sig_open)
		mvwchgat(_processes, _selected + 2, 1, x - 2, A_NORMAL, MY_LINE, nullptr);
	/*
	else
	{
		_selected = std::find(_procinfo.begin(), _procinfo.end(), _selected_proc) - _procinfo.begin();
		mvwchgat(_processes, _selected + 2, 1, x - 2, A_NORMAL, MY_ULINE, nullptr);
		getmaxyx(_signals, y, x);
		mvwchgat(_signals, _sig_selected + 2, 1, x - 2, A_NORMAL, MY_LINE, nullptr);
	}
	*/
}

// -----------------------------------------------------------------
static void	handle_up_vp_border(unsigned int &selector, unsigned int &start, unsigned int &finish)
{
	if (selector)
		--selector;
	else
	{
		if (start)
		{
			--start;
			--finish;
		}
	}
}

static void	handle_down_vp_border(unsigned int &selector, unsigned int &start, unsigned int &finish, int size)
{
	if (selector + start + 1 >= finish)
	{
		if (finish < size)
		{
			++finish;
			++start;
		}
	}
	else if (selector + 1 < size)
		++selector;
}
// -----------------------------------------------------------------

int		ProcessWindow::handle_input()
{
	switch (int c = wgetch(_processes))
	{
		case 'q': return (-1);
				  /*
		case 'k':
		{
			_open_signal_window();
			_selected_proc = _procinfo[_selected];
			break ;
		}
		*/
		case KEY_UP:
		{
			handle_up_vp_border(_selected, _vp_start, _vp_end);
			break ;
		}
		case KEY_DOWN:
		{
			handle_down_vp_border(_selected, _vp_start, _vp_end, _procinfo.size());
			break ;
		}
	}
	werase(_processes);
	wborder(_processes, '|', '|', '-', '-', '+', '+', '+', '+');
	// ~~~~~~~~~~~~~~~~~~~~~
	draw(_procinfo);
	// ~~~~~~~~~~~~~~~~~~~~~
	wrefresh(_processes);
	return (0);
}
