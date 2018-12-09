#ifndef ITOP_VISUAL_HPP
# define ITOP_VISUAL_HPP

# include <vector>
# include "SysInfo.hpp"
// interface for top visualisation

struct	Cpu_usage
{
	double	us;
	double	sy;
	double	ni;
	double	id;
	double	wa;
	double	hi;
	double	si;
	double	st;
	double	total;
};

class ITop_visual
{
	public:
		virtual void	display_top_info(std::string const &, long int, int, SysInfo::Load_avg const &) const = 0;
		virtual void	display_tasks_info(SysInfo::Tasks_count const &) const = 0;
		virtual void	display_cpu_info(Cpu_usage const &) const = 0;
		virtual void	display_mem_info(SysInfo::Meminfo const &) const = 0;
		virtual void	display_swap_info(SysInfo::Meminfo const &) const = 0;
		virtual void	display_procs_info(std::vector<SysInfo::Procinfo> const &) const = 0;
};
#endif
