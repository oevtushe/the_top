#ifndef ISYS_HPP
# define ISYS_HPP

# include "IIntermediate.hpp"
# include <string>
# include <vector>

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

		virtual									~ISys() {};
		std::vector<Procinfo_raw> const			&get_procs_data() const { return (_proc); }
		Cpuinfo const							&get_cpu_data() const { return (_cpu); }
		Meminfo const							&get_mem_data() const { return (_mem); }
		Tasks_count const						&get_tasks_count() const { return (_tasks_count); }
		Load_avg const							&get_loadavg() const { return (_load_avg); }
		std::string const						&get_curtime() const { return (_cur_time); }
		long int								get_uptime() const { return (_uptime); }
		int										get_num_of_users() const { return (_num_of_users); }
		void									update();
	protected:
		std::vector<Procinfo_raw>				_proc;
		Meminfo									_mem;
		Cpuinfo									_cpu;
		Load_avg								_load_avg;
		long int								_uptime;
		int										_num_of_users;
		std::string								_cur_time;
		Tasks_count								_tasks_count;
	private:
		virtual std::vector<Procinfo_raw>		_read_proc_data() const = 0;
		virtual Cpuinfo							_read_cpu_data() const = 0;
		virtual Meminfo							_read_mem_data() const = 0;
		virtual Load_avg						_read_loadavg_data() const = 0;
		virtual std::string						_read_cur_time() const = 0;
		virtual long int						_read_uptime() const = 0;
		virtual int								_read_num_of_users() const = 0;
		virtual Tasks_count						_calc_tasks() = 0;
};

#endif
