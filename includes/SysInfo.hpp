#ifndef SYSINFO_HPP
# define SYSINFO_HPP

# include <vector>
# include <string>
# include "ISys.hpp"

/*
** Method update() loads data from the system,
** further this class is used as database.
*/

class	SysInfo : public ISys
{
	public:
		SysInfo();
		~SysInfo() = default;
		std::vector<ISys::Procinfo_raw> const	&get_procs_data() const;
		ISys::Cpuinfo const						&get_cpu_data() const;
		ISys::Meminfo const						&get_mem_data() const;
		ISys::Tasks_count const					&get_tasks_count() const;
		ISys::Load_avg const					&get_loadavg() const;
		std::string const						&get_curtime() const;
		long int								get_uptime() const;
		int										get_num_of_users() const;
		void									update();
	private:
		std::vector<Procinfo_raw>				_read_proc_data() const;
		ISys::Procinfo_raw						_read_proc_data_hlp(std::string const path) const;
		ISys::Cpuinfo							_read_cpu_data(std::ifstream &fstat) const;
		ISys::Meminfo							_read_mem_data(std::ifstream &fmemi) const;
		ISys::Load_avg							_read_loadavg_data(std::ifstream &flavg) const;
		std::string								_read_cur_time() const;
		long int								_read_uptime() const;
		int										_read_num_of_users() const;
		void									_calc_tasks();
		std::vector<Procinfo_raw>				_proc;
		ISys::Meminfo							_mem;
		ISys::Cpuinfo							_cpu;
		ISys::Load_avg							_load_avg;
		long int								_uptime;
		int										_num_of_users;
		std::string								_cur_time;
		ISys::Tasks_count						_tasks_count;
};

#endif
