#ifndef THE_TOP_HPP
# define THE_TOP_HPP

# include "SysInfo.hpp"
# include <string>

namespace The_top
{
	struct	Cpu_usage
	{
		double	us;
		double	sy;
		double	ni;
		double	id;
		double	wa;
		double	hi;
		double	si;
		double	st;
		double	total;
	};

	/*
	** Here field 'cpu' is a percent of cpu usage
	** No more difference from SysInfo::Procinfo_raw
	*/

	struct Procinfo
	{
		friend bool		operator==(
			Procinfo const &a, Procinfo const &b);
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
		double			cpu;
	};

The_top::Cpu_usage				calc_cpu_usage(SysInfo::Cpuinfo const &prev,
			SysInfo::Cpuinfo const &cur);
std::vector<The_top::Procinfo>	get_procinfo(std::vector<SysInfo::Procinfo_raw> const &prev,
		std::vector<SysInfo::Procinfo_raw> const &cur, unsigned long int total);

}
#endif
