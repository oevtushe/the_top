#ifndef SIGNALSWINDOW_HPP
# define SIGNALSWINDOW_HPP

# include <ncurses.h>
# include <vector>
# include <utility>
# include <string>

class SignalsWindow
{
	enum Colors {MY_RED = 1, MY_BLUE, MY_GREEN, MY_YELLOW, MY_CYAN, MY_LINE, MY_ULINE, MY_HEADER};//
	public:
		SignalsWindow(int nlines, int ncols, int begin_y, int begin_x);
		~SignalsWindow();
		void	draw();
		void	clear();
		void	refresh();
		void	erase();
		void	handle_input(int pid);
		std::pair<int,int>	get_size();
		std::pair<int,int>	get_pos();
	private:
		WINDOW 									*_signals;
		std::vector<std::pair<int,std::string>>	_vsignals;
		unsigned int							_sig_start{};
		unsigned int							_sig_end;
		unsigned int							_sig_selected{};
		void									_init_signals();
		void									_display_cursor();
		std::pair<int,int>						_size;
		std::pair<int,int>						_pos;
		// maybe make base with movement already implemented
};

#endif
