#ifndef SYSINFO_HPP
# define SYSINFO_HPP

# include <vector>
# include "Task.hpp"

class	SysInfo
{
	public:
		SysInfo() = default;
		~SysInfo() = default;
		std::vector<Task>	get_tasks() const;
	private:
		Task				get_data(std::string const path) const;
};

#endif
