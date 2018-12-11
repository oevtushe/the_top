#ifndef ISYS_HPP
# define ISYS_HPP

# include "IIntermediate.hpp"
# include <string>

class	ISys : public IIntermediate
{
	public:
		struct Procinfo_raw
		{
			friend bool		operator==(Procinfo_raw const &a, Procinfo_raw const &b);
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
			unsigned long	cpu;
		};

		virtual 								~ISys() {};
		virtual std::vector<Procinfo_raw> const	&get_procs_data() const = 0;
		virtual Cpuinfo const					&get_cpu_data() const = 0;
		virtual Meminfo const					&get_mem_data() const = 0;
		virtual Tasks_count const				&get_tasks_count() const = 0;
		virtual Load_avg const					&get_loadavg() const = 0;
		virtual std::string const				&get_curtime() const = 0;
		virtual long int						get_uptime() const = 0;
		virtual int								get_num_of_users() const = 0;
		virtual void							update() = 0;
};

#endif
