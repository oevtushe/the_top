#include "SysInfo.hpp"
#include <iostream>
#include <string>
#include <vector>

int		main(void)
{
	SysInfo	si;
	std::vector<Task> data;

	data = si.get_tasks();
	for (Task const &t : data)
	{
		std::cout << t.get_pid() << " " << t.get_name() << " "
			<< t.get_state() << " " << t.get_nice() << " " << t.get_vmsize()
			<< " " << t.get_shr() << " " << t.get_res() << std::endl;
	}
	return (0);
}
