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

		struct Visual_db
		{
			Cpu_usage				usage;
			std::vector<Procinfo>	procinfo;
			Meminfo					meminfo;
			//Tasks_count				tasks_count;
			Load_avg				load_avg;
			int						threads;
			long int				uptime;
		};

		virtual						~IVisual() {}
		virtual void				clear() = 0;
		virtual void				draw(Visual_db const &db) = 0;
		virtual bool				wait() = 0;
};
#endif
