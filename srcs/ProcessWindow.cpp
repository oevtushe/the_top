#include "ProcessWindow.hpp"
#include <algorithm>
#include <unistd.h>

ProcessWindow::ProcessWindow(int nlines, int ncols, int begin_y, int begin_x) :
	_size{nlines, ncols}, _pos{begin_y, begin_x}
{
	_processes = newwin(nlines, ncols, begin_y, begin_x);
	_vp_end = nlines - 3;
	::keypad(_processes, TRUE);
}

ProcessWindow::~ProcessWindow()
{
	werase(_processes);
	wrefresh(_processes);
	delwin(_processes);
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

int		ProcessWindow::get_selected_pid()
{
	if (_procinfo.size() < _selected)
		return (_procinfo[_selected].pid);
	return (0);
}

void	ProcessWindow::resize(int nlines, int ncols, int begin_y, int begin_x)
{
	clear();
	delwin(_processes); // same as constructor
	_vp_end = nlines - 3;
	_processes = newwin(nlines, ncols, begin_y, begin_x);
	::keypad(_processes, TRUE);
	_size.first = nlines;
	_size.second = ncols;
	_pos.first = begin_y;
	_pos.second = begin_x;
}

// return const ref
std::pair<int,int>	ProcessWindow::get_size()
{
	return (_size);
}

std::pair<int,int>	ProcessWindow::get_pos()
{
	return (_pos);
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

void	ProcessWindow::_display_procs_info()
{
	const int tck_sc = sysconf(_SC_CLK_TCK);
	const int times = _vp_end > _procinfo.size() ? _procinfo.size() : _vp_end;

	_display_header();
	for (int i = _vp_start, j = 0; i < times; ++i, ++j)
	{
		mvwprintw(_processes, j + 2, 1, "%5d %-9.9s %2.3s %3d %7d %6d %6d "
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
	_saved_proc = _procinfo[_selected]; // is safe ?
	_freeze = true;
}

void	ProcessWindow::unfreeze()
{
	_freeze = false;
}

void	ProcessWindow::_display_cursor()
{
	int x, y;

	getmaxyx(_processes, y, x);
	if (_selected + _vp_start >= _procinfo.size())
		_selected = 0;
	if (!_freeze)
		mvwchgat(_processes, _selected + 2, 1, x - 2, A_NORMAL, MY_LINE, nullptr);
	else
	{
		_selected = std::find(_procinfo.begin(), _procinfo.end(), _saved_proc) - _procinfo.begin();
		mvwchgat(_processes, _selected + 2, 1, x - 2, A_NORMAL, MY_ULINE, nullptr);
	}
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

void	ProcessWindow::handle_input()
{
	switch (int c = wgetch(_processes))
	{
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
	erase();
	draw();
	refresh();
}
