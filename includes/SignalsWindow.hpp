#ifndef SIGNALSWINDOW_HPP
# define SIGNALSWINDOW_HPP

# include "Window.hpp"
# include "Selectable.hpp"
# include <ncurses.h>
# include <vector>
# include <utility>
# include <string>

class SignalsWindow : public Window, public Selectable
{
	public:
		SignalsWindow(int nlines, int ncols, int begin_y, int begin_x);
		void	draw();
		void	handle_input(int pid);
	private:
		std::vector<std::pair<int,std::string>>	_vsignals;
		void									_init_signals();
		void									_display_cursor();
};

#endif
