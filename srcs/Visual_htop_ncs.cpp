#include "Visual_htop_ncs.hpp"

#include <algorithm>

#include <ncurses.h>


bool	operator==(IVisual::Procinfo const &a,
			IVisual::Procinfo const &b)
{
	return (a.pid == b.pid);
}

Visual_htop_ncs::Visual_htop_ncs() : _pw{}
{
	::initscr();
	::start_color();
	::noecho();
	::cbreak();
	::curs_set(0);

	::refresh();
	::keypad(stdscr, TRUE);

	::init_pair(Window::MY_HEADER, COLOR_BLACK, COLOR_GREEN);
	::init_pair(Window::MY_RED, COLOR_RED, COLOR_BLACK);
	::init_pair(Window::MY_BLUE, COLOR_BLUE, COLOR_BLACK);
	::init_pair(Window::MY_GREEN, COLOR_GREEN, COLOR_BLACK);
	::init_pair(Window::MY_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	::init_pair(Window::MY_CYAN, COLOR_CYAN, COLOR_BLACK);
	::init_pair(Window::MY_LINE, COLOR_BLACK, COLOR_CYAN);
	::init_pair(Window::MY_ULINE, COLOR_BLACK, COLOR_WHITE);
	_fut = std::async(std::launch::async, &Visual_htop_ncs::_key_handler, this);
	_init_windows();
}

void	Visual_htop_ncs::_init_windows()
{
	// add space between windows if can't divide evenly
	const int space_between = COLS % 2 ? 2 : 1;
	const int align_left_right = 2;

	const int meters_height = 5; // additional 2 is borders
	const int meters_width = (COLS - align_left_right * 2 - space_between) / 2; // always divides evenly
																				// thanks to space_between
	const int meters_start_x = 2;
	const int meters_start_y = 1;
	_mw = new MetersWindow(meters_height, meters_width, meters_start_y, meters_start_x);

	const int text_info_start_x = meters_width + meters_start_x + space_between;
	_tiw = new TextInfoWindow(meters_height, meters_width, meters_start_y, text_info_start_x);

	const int processes_height = LINES - meters_height - meters_start_y;
	const int processes_width = COLS;
	const int proc_start_y = meters_height + meters_start_y;
	const int proc_start_x = 0;
	_pw = new ProcessWindow(processes_height, processes_width, proc_start_y, proc_start_x);

	const int sig_width = 16;
	_sw = new SignalsWindow(processes_height, sig_width, proc_start_y, proc_start_x);
}

void	Visual_htop_ncs::_del_wins()
{
	delete _tiw;
	delete _mw;
	delete _pw;
	delete _sw;
}

Visual_htop_ncs::~Visual_htop_ncs()
{
	_del_wins();
	::endwin();
}

bool	Visual_htop_ncs::wait()
{
	return (_fut.wait_for(std::chrono::seconds(_wait_sec)) == std::future_status::timeout);
}

void	Visual_htop_ncs::clear()
{
	_mw->erase();
	_tiw->erase();
	_pw->erase();
	if (_is_signals_visible)
		_sw->erase();
}

void	Visual_htop_ncs::_refresh()
{
	_mw->refresh();
	_tiw->refresh();
	_pw->refresh();
	if (_is_signals_visible)
		_sw->refresh();
}

void	Visual_htop_ncs::draw(Visual_db const &db)
{
		const int running = std::count_if(db.procinfo.begin(), db.procinfo.end(),
				[](IVisual::Procinfo const &p) { return (p.state == 'R'); });
		_tiw->draw(db.threads, db.procinfo.size(),
				running, db.load_avg, db.uptime);
		_mw->draw(db.meminfo, db.usage);
		_pw->set_data(db.procinfo);
		_pw->draw();
		if (_is_signals_visible)
			_sw->draw();
		_refresh();
}

void	Visual_htop_ncs::_resize()
{
	_del_wins();
	::refresh();
	_init_windows();
}

void	Visual_htop_ncs::_open_signals_window()
{
	const std::pair<int,int> proc_size{_pw->get_size()};
	const std::pair<int,int> proc_pos{_pw->get_pos()};
	const std::pair<int,int> sig_size{_sw->get_size()};
	const std::pair<int,int> sig_pos{_sw->get_pos()};

	// put process window right to signal window
	_pw->resize(proc_size.first, COLS - sig_size.second,
			proc_pos.first, proc_pos.second + sig_size.second);
	_sw->draw();
	_sw->refresh();
	_pw->draw();
	_pw->refresh();
}

void	Visual_htop_ncs::_key_handler()
{
	while (true)
	{
		switch (int c = getch())
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
				{
					const int pid = _pw->get_selected_pid();
					if (pid > 0)
						_sw->handle_input(pid);
				}
		}
	}
}
