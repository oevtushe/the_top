#ifndef PROCESSWINDOW_HPP
# define PROCESSWINDOW_HPP

# include "IVisual.hpp"
# include "Window.hpp"
# include "Selectable.hpp"

class ProcessWindow : public Window, public Selectable
{
	public:
		ProcessWindow(int nlines, int ncols, int begin_y, int begin_x);
		void	draw();
		void	handle_input();
		void	freeze();
		void	unfreeze();
		void	resize(int nlines, int ncols, int begin_y, int begin_x);
		void	set_data(std::vector<IVisual::Procinfo> const &procinfo);
		int		get_selected_pid();
	private:
		void									_display_header();
		void									_display_cursor();
		void									_display_procs_info();
		IVisual::Procinfo						_saved_proc{};
		std::vector<IVisual::Procinfo> 			_procinfo;
		IVisual::Procinfo						_selected_proc;
		bool									_freeze{};
};

#endif
