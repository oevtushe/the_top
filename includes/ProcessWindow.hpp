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
		void	draw();
		void	handle_input();
		void	erase();
		void	refresh();
		void	clear();
		void	freeze();
		void	unfreeze();
		void	resize(int nlines, int ncols, int begin_y, int begin_x);
		void	set_data(std::vector<IVisual::Procinfo> const &procinfo);
		int		get_selected_pid();
		std::pair<int,int>	get_size();
		std::pair<int,int>	get_pos();
	private:
		void									_display_header();
		void									_display_cursor();
		void									_display_procs_info();
		unsigned int							_vp_start{}; // vp -> view point
		unsigned int							_vp_end;
		unsigned int							_selected{};
		IVisual::Procinfo						_saved_proc;
		WINDOW									*_processes;
		std::vector<IVisual::Procinfo> 			_procinfo;
		IVisual::Procinfo						_selected_proc;
		std::pair<int,int>						_size;
		std::pair<int,int>						_pos;
		bool									_freeze{};
};

#endif
