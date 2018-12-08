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
			unsigned long int buffer;
			unsigned long int cache;
			unsigned long int sreclaimable;
			unsigned long int mem_total;
			unsigned long int mem_free;
			unsigned long int swap_total;
			unsigned long int swap_free;
		};

		// info about single process
		struct Procinfo
		{
			int				pid;
			std::string		command;
			char			state;
			long int		nice;
			long int		priority;
			long int		vsize;
			long int		rss;
			long int		mem_shared;
			std::string		user;
			unsigned long	utime;
			unsigned long	stime;
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

		SysInfo() = default;
		~SysInfo() = default;
		std::vector<Procinfo> const	&get_procs_data() const;
		Cpuinfo const				&get_cpu_data() const;
		Meminfo const				&get_mem_data() const;
		std::array<double, 3> const	&get_loadavg() const;
		std::string const			&get_curtime() const;
		long int					get_uptime() const;
		int							get_num_of_users() const;
		void						update();
	private:
		std::vector<Procinfo>		_read_proc_data() const;
		Procinfo					_read_proc_data_hlp(std::string const path) const;
		Cpuinfo						_read_cpu_data(std::ifstream &fstat) const;
		Meminfo						_read_mem_data(std::ifstream &fmemi) const;
		std::array<double, 3>		_read_loadavg_data(std::ifstream &flavg) const;
		std::string					_read_cur_time() const;
		long int					_read_uptime() const; //general
		int							_read_num_of_users() const;
		std::vector<Procinfo>		_proc;
		Meminfo						_mem;
		Cpuinfo						_cpu;
		std::array<double, 3>		_load_avg;
		long int					_uptime;
		int							_num_of_users;
		std::string					_cur_time;
};

#endif
