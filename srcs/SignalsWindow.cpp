#include "SignalsWindow.hpp"

#include <ncurses.h>
#include <signal.h>

SignalsWindow::SignalsWindow(int nlines, int ncols, int begin_y, int begin_x) :
	Window(nlines, ncols, begin_y, begin_x)
{
	_init_signals();
	_vp_end = nlines - 3;
	::keypad(_win, TRUE);
}

void	SignalsWindow::_init_signals()
{
	_vsignals.push_back({SIGHUP, "SIGHUP"});
	_vsignals.push_back({SIGINT, "SIGINT"});
	_vsignals.push_back({SIGQUIT, "SIGQUIT"});
	_vsignals.push_back({SIGILL, "SIGILL"});
	_vsignals.push_back({SIGABRT, "SIGABRT"});
	_vsignals.push_back({SIGFPE, "SIGFPE"});
	_vsignals.push_back({SIGKILL, "SIGKILL"});
	_vsignals.push_back({SIGSEGV, "SIGSEGV"});
	_vsignals.push_back({SIGPIPE, "SIGPIPE"});
	_vsignals.push_back({SIGALRM, "SIGALRM"});
	_vsignals.push_back({SIGTERM, "SIGTERM"});
	_vsignals.push_back({SIGUSR1, "SIGUSR1"});
	_vsignals.push_back({SIGUSR2, "SIGUSR2"});
	_vsignals.push_back({SIGCHLD, "SIGCHLD"});
	_vsignals.push_back({SIGCONT, "SIGCONT"});
	_vsignals.push_back({SIGSTOP, "SIGSTOP"});
	_vsignals.push_back({SIGTSTP, "SIGTSTP"});
	_vsignals.push_back({SIGTTIN, "SIGTTIN"});
	_vsignals.push_back({SIGTTOU, "SIGTTOU"});
}

void	SignalsWindow::draw()
{
	int		times = _vp_end > _vsignals.size() ? _vsignals.size() : _vp_end;
	mvwprintw(_win, 1, 1, "Send signal:");
	for (int j = _vp_start, i = 2; j < times; ++j, ++i)
	{
		mvwprintw(_win, i, 1, "%s", _vsignals[j].second.c_str());
	}
	mvwchgat(_win, 1, 1, _size.second - 2, A_NORMAL, MY_HEADER, nullptr); // highlight header
	_display_cursor();
	wrefresh(_win);
}

void	SignalsWindow::_display_cursor()
{
	mvwchgat(_win, _selected + 2, 1, _size.second - 2, A_NORMAL, MY_LINE, nullptr);
}


void	SignalsWindow::handle_input(int pid) // similar to ProcessWindow
{
	switch (wgetch(_win))
	{
		case KEY_UP:
		{
			_handle_up_vp_border();
			break ;
		}
		case KEY_DOWN:
		{
			_handle_down_vp_border(_vsignals.size());
			break ;
		}
		case 10: // Enter
		{
			kill(pid, _vsignals[_vp_start + _selected].first);
			ungetch('q'); // close this window
			break ;
		}
	}
	erase();
	draw();
	refresh();
}
