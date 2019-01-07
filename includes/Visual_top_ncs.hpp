#ifndef VISUAL_TOP_NCS_HPP
# define VISUAL_TOP_NCS_HPP

# include "IVisual.hpp"

class Visual_top_ncs : public IVisual
{
	public:
		Visual_top_ncs();
		~Visual_top_ncs();
		Visual_top_ncs(Visual_top_ncs const &) = delete;
		Visual_top_ncs(Visual_top_ncs &&) = delete;
		void	clear();
		void	draw(Visual_db const &db);
		bool	wait();
	private:
		int		_offset;
		void	_display_top_info(std::string const &, long int, int, IVisual::Load_avg const &) const;
		void	_display_tasks_info(IVisual::Tasks_count const &) const;
		void	_display_cpu_info(IVisual::Cpu_usage const &) const;
		void	_display_mem_info(IVisual::Meminfo const &) const;
		void	_display_swap_info(IVisual::Meminfo const &) const;
		void	_display_procs_info(std::vector<IVisual::Procinfo> const &);
};

#endif
