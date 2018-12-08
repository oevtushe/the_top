#ifndef THE_TOP_HPP
# define THE_TOP_HPP

# include "SysInfo.hpp"

namespace The_top
{
	struct	Top_info
	{
		std::string	cur_time;
		long int	uptime;
		int			num_of_users;
		double		la_1;
		double		la_5;
		double		la_15;
	};

	struct	Tasks_info
	{
		int	total;
		int	running;
		int	sleeping;
		int	stopped;
		int	zombie;
	};

	struct	Cpu_info
	{
		double	us;
		double	sy;
		double	ni;
		double	id;
		double	wa;
		double	hi;
		double	si;
		double	st;
	};

	struct	Mem_info
	{
		unsigned long int	total;
		unsigned long int	free;
		unsigned long int	bc;
		unsigned long int	used;
	};

	struct	Swap_info
	{
		unsigned long int	total;
		unsigned long int	free;
		unsigned long int	used;
		unsigned long int	avail;
	};

	struct Proc_info
	{
		int				pid;
		std::string		command;
		char			state;
		long int		nice;
		long int		priority;
		long int		vsize;
		long int		res;
		long int		shared;
		std::string		user;
		long int		timep;
		double			cpu;
		double			mem;
	};

	void	init_top_info(Top_info &ti, SysInfo const &si);
	void	init_tasks_info(Tasks_info &ti, SysInfo const &si);
	void	init_cpu_info();
	void	init_mem_info(Mem_info &ti, SysInfo const &si);
	void	init_swap_info(Swap_info &ti, SysInfo const &si);
	void	init_procs_info(std::vector<Proc_info> &pi, SysInfo const &si);
}
#endif
