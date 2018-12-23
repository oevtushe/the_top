#ifndef VISUAL_NCS_HPP
# define VISUAL_NCS_HPP

# include "IVisual.hpp"
# include <ncurses.h>

class Visual_ncs : public IVisual
{
	public:
		Visual_ncs();
		~Visual_ncs();
		Visual_ncs(Visual_ncs const &) = delete;
		Visual_ncs(Visual_ncs &&) = delete;
		void	refresh() const;
		void	clean_screen() const;
		int		read_ch();
		void	display_cpu_bar(IVisual::Cpu_usage const &usage) const;
		void	display_mem_bar(IVisual::Meminfo const &memi) const;
		void	display_procs_info(std::vector<IVisual::Procinfo> const &);
		void	display_right_window(int threads, int tasks, int r,
			IVisual::Load_avg const &load_avg, long int uptime);
		void	display_swap_bar(IVisual::Meminfo const &memi) const;
	private:
		int		_selected;
		WINDOW	*_meters;
		WINDOW	*_text_info;
		WINDOW	*_processes;
};

#endif
