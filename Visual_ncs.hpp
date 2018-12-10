#ifndef VISUAL_NCS_HPP
# define VISUAL_NCS_HPP

# include "The_top.hpp"
# include "ITop_visual.hpp"

class Visual_ncs : public ITop_visual
{
	public:
		Visual_ncs();
		~Visual_ncs();
		Visual_ncs(Visual_ncs const &) = delete;
		Visual_ncs(Visual_ncs &&) = delete;
		void	refresh() const;
		void	clean_screen() const;
		int		read_ch() const;
		void	display_top_info(std::string const &, long int, int, SysInfo::Load_avg const &) const;
		void	display_tasks_info(SysInfo::Tasks_count const &) const;
		void	display_cpu_info(The_top::Cpu_usage const &) const;
		void	display_mem_info(SysInfo::Meminfo const &) const;
		void	display_swap_info(SysInfo::Meminfo const &) const;
		void	display_procs_info(std::vector<The_top::Procinfo> const &) const;
};

#endif
