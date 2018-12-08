#ifndef ITOP_VISUAL_HPP
# define ITOP_VISUAL_HPP

# include "The_top.hpp"
// interface for top visualisation
class ITop_visual
{
	public:
		virtual	void	display_top_info(The_top::Top_info const &) const = 0;
		virtual	void	display_tasks_info(The_top::Tasks_info const &) const = 0;
		virtual	void	display_cpu_info(The_top::Cpu_info const &) const = 0;
		virtual	void	display_mem_info(The_top::Mem_info const &) const = 0;
		virtual	void	display_swap_info(The_top::Swap_info const &) const = 0;
		virtual	void	display_procs_info(The_top::Proc_info const &) const = 0;
};
#endif
