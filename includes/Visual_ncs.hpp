#ifndef VISUAL_NCS_HPP
# define VISUAL_NCS_HPP

# include "IVisual.hpp"

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
		void	display_top_info(std::string const &, long int, int, IVisual::Load_avg const &) const;
		void	display_tasks_info(IVisual::Tasks_count const &) const;
		void	display_cpu_info(IVisual::Cpu_usage const &) const;
		void	display_mem_info(IVisual::Meminfo const &) const;
		void	display_swap_info(IVisual::Meminfo const &) const;
		void	display_procs_info(std::vector<IVisual::Procinfo> const &);
	private:
		int		_selected;
};

#endif
