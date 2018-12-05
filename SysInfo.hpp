#ifndef SYSINFO_HPP
# define SYSINFO_HPP

# include <map>
# include <vector>

class	SysInfo
{
	public:
		using	ProcessDB = std::map<std::string, std::string>;
		using	GenDB = std::map<std::string, std::string>;
		using	ProcessesDB = std::vector<ProcessDB>;

		SysInfo() = default;
		~SysInfo() = default;
		ProcessesDB	get_tasks() const;
		GenDB		get_gen_data() const;
	private:
		ProcessDB	_get_proc_data(std::string const path) const;
};

#endif
