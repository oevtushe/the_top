#include "ISys.hpp"

void	ISys::update()
{
	_cpu = _read_cpu_data();
	_mem = _read_mem_data();
	_proc = _read_proc_data();
	_load_avg = _read_loadavg_data();
	_uptime = _read_uptime();
	_num_of_users = _read_num_of_users();
	_cur_time = _read_cur_time();
	_tasks_count = _calc_tasks();
}
