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
	private:
		std::vector<Procinfo_raw>				_read_proc_data();
		ISys::Procinfo_raw						_read_proc_data_hlp(std::ifstream &fstat, std::ifstream &fstatm, std::string const &path);
		ISys::Cpuinfo							_read_cpu_data() const;
		ISys::Meminfo							_read_mem_data() const;
		ISys::Load_avg							_read_loadavg_data() const;
		std::string								_read_cur_time() const;
		long int								_read_uptime() const;
		int										_read_num_of_users() const;
		ISys::Tasks_count						_calc_tasks();
};

#endif
