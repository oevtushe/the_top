#include "Window.hpp"

Window::Window(int nlines, int ncols, int begin_y, int begin_x) :
	_size{nlines, ncols}, _pos{begin_y, begin_x}
{
	_win = newwin(nlines, ncols, begin_y, begin_x);
}

Window::~Window()
{
	clear();
	delwin(_win);
}

std::pair<int,int> const &Window::get_size() const
{
	return (_size);
}

std::pair<int,int> const &Window::get_pos() const
{
	return (_pos);
}

void					Window::refresh()
{
	wrefresh(_win);
}

void					Window::clear()
{
	erase();
	wrefresh(_win);
}

void					Window::erase()
{
	werase(_win);
}
