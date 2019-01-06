#include "SignalsWindow.hpp"
#include <signal.h>

SignalsWindow::SignalsWindow(int nlines, int ncols, int begin_y, int begin_x) :
	_size{nlines, ncols}, _pos{begin_y, begin_x}
{
	_signals = newwin(nlines, ncols, begin_y, begin_x);
	_init_signals();
	_sig_end = nlines - 3;
	::keypad(_signals, TRUE);
}

SignalsWindow::~SignalsWindow()
{
	delwin(_signals);
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

// return const ref
std::pair<int,int>	SignalsWindow::get_size()
{
	return (_size);
}

std::pair<int,int>	SignalsWindow::get_pos()
{
	return (_pos);
}

void	SignalsWindow::clear()
{
	erase();
	refresh();
}

void	SignalsWindow::erase()
{
	werase(_signals);
}

void	SignalsWindow::refresh()
{
	wrefresh(_signals);
}

void	SignalsWindow::draw()
{
	int		times = _sig_end > _vsignals.size() ? _vsignals.size() : _sig_end;
	mvwprintw(_signals, 1, 1, "Send signal:");
	for (int j = _sig_start, i = 2; j < times; ++j, ++i)
	{
		mvwprintw(_signals, i, 1, "%s", _vsignals[j].second.c_str());
	}
	int x, y; // have vars for that
	getmaxyx(_signals, y, x);
	mvwchgat(_signals, 1, 1, x - 2, A_NORMAL, MY_HEADER, nullptr); // highlight header
	_display_cursor();
	wrefresh(_signals);
}

void	SignalsWindow::_display_cursor()
{
	int x, y;

	getmaxyx(_signals, y, x); // have a var for that
	mvwchgat(_signals, _sig_selected + 2, 1, x - 2, A_NORMAL, MY_LINE, nullptr);
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

void	SignalsWindow::handle_input(int pid) // similar to ProcessWindow
{
	switch (int c = wgetch(_signals))
	{
		case KEY_UP:
		{
			handle_up_vp_border(_sig_selected, _sig_start, _sig_end);
			break ;
		}
		case KEY_DOWN:
		{
			handle_down_vp_border(_sig_selected, _sig_start, _sig_end, _vsignals.size());
			break ;
		}
		case 10: // Enter
		{
			kill(pid, _vsignals[_sig_selected].first);
			ungetch('q'); // close this window
			break ;
		}
	}
	erase();
	draw();
	refresh();
}
