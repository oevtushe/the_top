#ifndef PROCESSWINDOW_HPP
# define PROCESSWINDOW_HPP

# include "IVisual.hpp"
# include <ncurses.h>

class ProcessWindow
{
	public:
		enum Colors {MY_RED = 1, MY_BLUE, MY_GREEN, MY_YELLOW, MY_CYAN, MY_LINE, MY_ULINE, MY_HEADER};//
		ProcessWindow(int nlines, int ncols, int begin_y, int begin_x);
		~ProcessWindow();
		void	draw(std::vector<IVisual::Procinfo> const &pi);
		int		handle_input();
		void	erase();
		void	refresh();
		void	clear();
	private:
		void									_display_header();
		void									_display_cursor();
		void									_display_procs_info(std::vector<IVisual::Procinfo> const &pi);
		unsigned int							_vp_start; // vp -> view point
		unsigned int							_vp_end;
		unsigned int							_selected{};
		WINDOW									*_processes;
		std::vector<IVisual::Procinfo> 			_procinfo;
		IVisual::Procinfo						_selected_proc;
};

#endif
