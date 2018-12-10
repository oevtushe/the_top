#ifndef VISUAL_NCS_HPP
# define VISUAL_NCS_HPP

# include "ITop_visual.hpp"

class Visual_ncs : public ITop_visual
{
	public:
		Visual_ncs();
		~Visual_ncs();
		Visual_ncs(Visual_ncs const &) = delete;
		Visual_ncs(Visual_ncs &&) = delete;
		void	display_top_info(std::string const &, long int, int, SysInfo::Load_avg const &) const;
		void	display_tasks_info(SysInfo::Tasks_count const &) const;
		void	display_cpu_info(Cpu_usage const &) const;
		void	display_mem_info(SysInfo::Meminfo const &) const;
		void	display_swap_info(SysInfo::Meminfo const &) const;
		void	display_procs_info(std::vector<SysInfo::Procinfo> const &) const;
};
#endif
