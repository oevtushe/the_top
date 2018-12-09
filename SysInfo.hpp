#ifndef SYSINFO_HPP
# define SYSINFO_HPP

# include <map>
# include <vector>

class	SysInfo
{
	public:
		struct	Meminfo
		{
			unsigned long int available;
			unsigned long int bc;
			unsigned long int mem_total;
			unsigned long int mem_free;
			unsigned long int mem_used;
			unsigned long int swap_total;
			unsigned long int swap_free;
			unsigned long int swap_used;
		};

		// info about single process
		struct Procinfo
		{
			bool	operator==(Procinfo const &p)
			{
				return (p.pid == pid);
			}
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
			double			pcpu;
		};

		struct Cpuinfo
		{
			long int	user;
			long int	nice;
			long int	system;
			long int	idle;
			long int	iowait;
			long int	irq;
			long int	softirq;
			long int	steal;
			long int	total;
		};

		struct	Tasks_count
		{
			int	total;
			int	running;
			int	sleeping;
			int	stopped;
			int	zombie;
		};

		struct Load_avg
		{
			double		la_1;
			double		la_5;
			double		la_15;
		};

		SysInfo();
		~SysInfo() = default;
		std::vector<Procinfo> const	&get_procs_data() const;
		Cpuinfo const				&get_cpu_data() const;
		Meminfo const				&get_mem_data() const;
		Tasks_count const			&get_tasks_count() const;
		Load_avg const				&get_loadavg() const;
		std::string const			&get_curtime() const;
		long int					get_uptime() const;
		int							get_num_of_users() const;
		void						update();
	private:
		std::vector<Procinfo>		_read_proc_data() const;
		Procinfo					_read_proc_data_hlp(std::string const path) const;
		Cpuinfo						_read_cpu_data(std::ifstream &fstat) const;
		Meminfo						_read_mem_data(std::ifstream &fmemi) const;
		Load_avg					_read_loadavg_data(std::ifstream &flavg) const;
		std::string					_read_cur_time() const;
		long int					_read_uptime() const;
		int							_read_num_of_users() const;
		void						_calc_tasks();
		std::vector<Procinfo>		_proc;
		Meminfo						_mem;
		Cpuinfo						_cpu;
		Load_avg					_load_avg;
		long int					_uptime;
		int							_num_of_users;
		std::string					_cur_time;
		Tasks_count					_tasks_count;
		//Cpu_usage					_cpu_usage;
};

#endif
