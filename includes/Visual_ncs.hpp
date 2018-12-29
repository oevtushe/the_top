#ifndef VISUAL_NCS_HPP
# define VISUAL_NCS_HPP

# include "IVisual.hpp"
# include <ncurses.h>
# include <thread> //

class Visual_ncs : public IVisual
{
	public:
		enum Colors {MY_RED = 1, MY_BLUE, MY_GREEN, MY_YELLOW, MY_CYAN, MY_LINE, MY_ULINE, MY_HEADER};
		Visual_ncs();
		~Visual_ncs();
		Visual_ncs(Visual_ncs const &) = delete;
		Visual_ncs(Visual_ncs &&) = delete;
		void				refresh() const;
		void				clean_screen() const;
		void				display_cpu_bar(IVisual::Cpu_usage const &usage);
		void				display_mem_bar(IVisual::Meminfo const &memi);
		void				display_procs_info(std::vector<IVisual::Procinfo> const &);
		void				display_right_window(int threads, int tasks, int r,
			IVisual::Load_avg const &load_avg, long int uptime);
		void				display_swap_bar(IVisual::Meminfo const &memi);
		void				display_meter(int cp, int times);
		std::future<void>	run_key_handler();
		void				draw_screen(Visual_db const &db);
	private:
		unsigned int					_sig_selected{};
		unsigned int					_selected{};
		unsigned int					_vp_start; // vp -> view point
		unsigned int					_vp_end;
		WINDOW							*_meters;
		WINDOW							*_text_info;
		WINDOW							*_processes;
		WINDOW							*_signals;
		bool							_is_sig_open;
		std::vector<IVisual::Procinfo>	_procinfo;
		void							_key_handler();
		void							_display_cursor();
		void							_init_windows();
		void							_del_wins();
		void							_handle_up_vp_border();
		void							_handle_down_vp_border();
		void							_display_header();
		void							_open_signal_window();
		void							_close_signal_window();
		void							_draw_signal_win();
		int								_key_processes(int c);
		int								_key_signals(int c);
		int								_get_selected_pid();
};

#endif
