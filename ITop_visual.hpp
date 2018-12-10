#ifndef ITOP_VISUAL_HPP
# define ITOP_VISUAL_HPP

# include "SysInfo.hpp"
# include <vector>

/*
** An interface for the_top visualization
*/

class ITop_visual
{
	public:
		virtual	void	refresh() const = 0;
		virtual void	clean_screen() const = 0;
		virtual void	display_top_info(std::string const &, long int, int, SysInfo::Load_avg const &) const = 0;
		virtual void	display_tasks_info(SysInfo::Tasks_count const &) const = 0;
		virtual void	display_cpu_info(The_top::Cpu_usage const &) const = 0;
		virtual void	display_mem_info(SysInfo::Meminfo const &) const = 0;
		virtual void	display_swap_info(SysInfo::Meminfo const &) const = 0;
		virtual void	display_procs_info(std::vector<The_top::Procinfo> const &) const = 0;
};
#endif
