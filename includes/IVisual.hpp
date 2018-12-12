#ifndef IVISUAL_HPP
# define IVISUAL_HPP

# include "IIntermediate.hpp"
# include <vector>
# include <string>

/*
** An interface for the_top visualization
*/

class IVisual : public IIntermediate
{
	public:
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
			long int		vsize;
			long int		rss;
			long int		mem_shared;
			std::string		user;
			unsigned long	timep;
			double			memp;
			double			cpu;
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
