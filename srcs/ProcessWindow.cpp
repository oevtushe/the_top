#include "ProcessWindow.hpp"

#include <unistd.h>

#include <algorithm>
#include <limits>

# include <ncurses.h>

ProcessWindow::ProcessWindow(int nlines, int ncols, int begin_y, int begin_x) :
	Window(nlines, ncols, begin_y, begin_x)
{
	_vp_end = nlines - 3;
	::keypad(_win, TRUE);
}

void	ProcessWindow::set_data(std::vector<IVisual::Procinfo> const &procinfo)
{
	_procinfo = procinfo;
}

void	ProcessWindow::draw()
{
	_display_header();
	_display_procs_info();
	_display_cursor();
}

int		ProcessWindow::get_selected_pid()
{
	if (_selected < _procinfo.size())
		return (_procinfo[_selected].pid);
	return (std::numeric_limits<int>::max());
}

void	ProcessWindow::resize(int nlines, int ncols, int begin_y, int begin_x)
{
	clear();
	delwin(_win); // same as constructor
	_vp_end = nlines - 3;
	_win = newwin(nlines, ncols, begin_y, begin_x);
	::keypad(_win, TRUE);
	_size.first = nlines;
	_size.second = ncols;
	_pos.first = begin_y;
	_pos.second = begin_x;
}

void	ProcessWindow::_display_header()
{
	mvwprintw(_win, 1, 1, "%5.5s %-9.9s %2.2s %3.3s %7.7s %6.6s "
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
	getmaxyx(_win, y, x);
	mvwchgat(_win, 1, 1, x - 2, A_NORMAL, MY_HEADER, nullptr);
	wmove(_win, 1, 1);
}

void	ProcessWindow::_display_procs_info()
{
	const int tck_sc = sysconf(_SC_CLK_TCK);
	const int times = _vp_end > _procinfo.size() ? _procinfo.size() : _vp_end;

	_display_header();
	for (int i = _vp_start, j = 0; i < times; ++i, ++j)
	{
		mvwprintw(_win, j + 2, 1, "%5d %-9.9s %2.3s %3d %7d %6d %6d "
					"%c %4.1f %4.1f %3.1lu:%.2lu.%.2lu %-s",
				_procinfo[i].pid,
				_procinfo[i].user.c_str(),
				_procinfo[i].priority < -99 ? "rt" :
						std::to_string(_procinfo[i].priority).c_str(),
				_procinfo[i].nice,
				_procinfo[i].vsize,
				_procinfo[i].rss,
				_procinfo[i].mem_shared,
				_procinfo[i].state,
				_procinfo[i].cpu,
				_procinfo[i].memp,
				_procinfo[i].timep / (60 * tck_sc), // minutes
				(_procinfo[i].timep % 6000) / tck_sc, // seconds
				(_procinfo[i].timep % 6000) % 100, // 1/100 second
				_procinfo[i].command.c_str());
	}
	//_procinfo = pi; // steal data for key_handler
}

void	ProcessWindow::freeze()
{
	if (_selected < _procinfo.size())
		_saved_proc = _procinfo[_selected];
	_freeze = true;
}

void	ProcessWindow::unfreeze()
{
	_freeze = false;
}

void	ProcessWindow::_display_cursor()
{
	int x, y;

	getmaxyx(_win, y, x);
	if (_selected + _vp_start >= _procinfo.size())
		_selected = 0;
	if (!_freeze)
		mvwchgat(_win, _selected + 2, 1, x - 2, A_NORMAL, MY_LINE, nullptr);
	else
	{
		_selected = std::find(_procinfo.begin(), _procinfo.end(), _saved_proc) - _procinfo.begin();
		mvwchgat(_win, _selected + 2, 1, x - 2, A_NORMAL, MY_ULINE, nullptr);
	}
}

void	ProcessWindow::handle_input()
{
	switch (int c = wgetch(_win))
	{
		case KEY_UP:
		{
			_handle_up_vp_border();
			break ;
		}
		case KEY_DOWN:
		{
			_handle_down_vp_border(_procinfo.size());
			break ;
		}
	}
	erase();
	draw();
	refresh();
}
