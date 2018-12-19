#ifndef IVISUAL_HPP
# define IVISUAL_HPP

# include <vector>
# include <string>
# include "IIntermediate.hpp"

/*
** An interface for the_top visualization
*/

class IVisual : public IIntermediate
{
	public:
		struct	Cpu_usage
		{
			double	us; // user
			double	sy; // system
			double	ni; // nice
			double	id; // idle
			double	wa; // IO-wait
			double	hi; // hardware interrupts
			double	si; // software interrupts
			double	st; // stolen time by hypervisor
			double	total;
		};

		/*
		** Here field 'cpu' is a percent of cpu usage
		** No more difference from SysInfo::Procinfo_raw
		*/

		struct Procinfo
		{
			friend bool		operator==(
					Procinfo const &a, Procinfo const &b);
			int				pid;
			std::string		command;
			char			state;
			long int		nice;
			long int		priority;
			long int		vsize; // virtual memory size
			long int		rss; // Resident Set Size
			long int		mem_shared;
			std::string		user; // Owner
			unsigned long	timep; // utime + stime
			double			memp; // rss / Meminfo::mem_total
			double			cpu; // percentage of cpu usage
		};

		virtual			~IVisual() {}
		virtual	void	refresh() const = 0;
		virtual void	clean_screen() const = 0;
		virtual int		read_ch() = 0;
		virtual void	display_top_info(std::string const &, long int, int,
							Load_avg const &) const = 0;
		virtual void	display_tasks_info(Tasks_count const &) const = 0;
		virtual void	display_cpu_info(Cpu_usage const &) const = 0;
		virtual void	display_mem_info(Meminfo const &) const = 0;
		virtual void	display_swap_info(Meminfo const &) const = 0;
		virtual void	display_procs_info(std::vector<Procinfo> const &) = 0;
};
#endif
