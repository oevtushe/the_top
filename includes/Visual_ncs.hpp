#ifndef VISUAL_NCS_HPP
# define VISUAL_NCS_HPP

# include "IVisual.hpp"
# include "SignalsWindow.hpp"
# include "ProcessWindow.hpp"
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
		void									refresh() const;
		void									clean_screen() const;
		void									display_cpu_bar(IVisual::Cpu_usage const &usage);
		void									display_mem_bar(IVisual::Meminfo const &memi);
		void									display_right_window(int threads, int tasks, int r,
														IVisual::Load_avg const &load_avg, long int uptime);
		void									display_swap_bar(IVisual::Meminfo const &memi);
		void									display_meter(int cp, int times);
		std::future<void>						run_key_handler();
		void									draw_screen(Visual_db const &db);
	private:
		WINDOW									*_meters;
		WINDOW									*_text_info;
		bool									_is_signals_visible{};
		ProcessWindow							*_pw;
		SignalsWindow							*_sw;
		void									_key_handler();
		void									_init_windows();
		void									_del_wins();
		void									_resize();
		void									_open_signals_window();
};

#endif
