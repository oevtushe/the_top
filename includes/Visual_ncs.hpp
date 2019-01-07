#ifndef VISUAL_NCS_HPP
# define VISUAL_NCS_HPP

# include "IVisual.hpp"
# include "SignalsWindow.hpp"
# include "ProcessWindow.hpp"
# include "MetersWindow.hpp"
# include "TextInfoWindow.hpp"
# include <ncurses.h>
# include <thread> //
# include <future> //

class Visual_ncs : public IVisual
{
	public:
		Visual_ncs();
		~Visual_ncs();
		Visual_ncs(Visual_ncs const &) = delete;
		Visual_ncs(Visual_ncs &&) = delete;
		void									clear();
		bool									wait();
		void									draw(Visual_db const &db);
	private:
		void									_refresh();
		TextInfoWindow							*_tiw;
		bool									_is_signals_visible{};
		ProcessWindow							*_pw;
		SignalsWindow							*_sw;
		MetersWindow							*_mw;
		void									_key_handler();
		void									_init_windows();
		void									_del_wins();
		void									_resize();
		void									_open_signals_window();
		std::future<void>						_fut;
		int										_wait_sec{3};
};

#endif
