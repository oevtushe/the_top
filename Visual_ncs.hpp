#ifndef VISUAL_NCS_HPP
# define VISUAL_NCS_HPP

# include "ITop_visual.hpp"

class Visual_ncs : public ITop_visual
{
		void	display_top_info(The_top::Top_info const &) const;
		void	display_tasks_info(The_top::Tasks_info const &) const;
		void	display_cpu_info(The_top::Cpu_info const &) const;
		void	display_mem_info(The_top::Mem_info const &) const;
		void	display_swap_info(The_top::Swap_info const &) const;
		void	display_procs_info(The_top::Proc_info const &) const;
};
#endif
