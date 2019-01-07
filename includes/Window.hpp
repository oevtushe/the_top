#ifndef WINDOW_HPP
# define WINDOW_HPP

# include <utility>

# include <ncurses.h>

class Window
{
	public:
		enum Colors {MY_RED = 1, MY_BLUE, MY_GREEN, MY_YELLOW, MY_CYAN, MY_LINE, MY_ULINE, MY_HEADER};
		Window(int nlines, int ncols, int begin_y, int begin_x);
		virtual 			~Window();
		void				refresh();
		//virtual void		draw() = 0; :(
		void				clear();
		void				erase();
		std::pair<int,int> const &get_size() const;
		std::pair<int,int> const &get_pos() const;
	protected:
		WINDOW				*_win;
		std::pair<int,int>	_size;
		std::pair<int,int>	_pos;
};

#endif
