#ifndef IINTERMEDIATE_HPP
# define IINTERMEDIATE_HPP

/*
** Holds structs used by both 'ISys' and 'IVisual'.
** Isn't a part of program logical structure.
*/

class IIntermediate
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
		struct Cpuinfo
		{
			long int user;
			long int nice;
			long int system;
			long int idle;
			long int iowait;
			long int irq;
			long int softirq;
			long int steal;
			long int total;
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
			double	la_1;
			double	la_5;
			double	la_15;
		};
};

#endif
