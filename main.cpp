#include "SysInfo.hpp"
#include <utmp.h>
#include <sys/sysinfo.h>
#include <ncurses.h>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <unistd.h>
#include <fstream>

int		get_users()
{
	FILE *f{};
	struct utmp tmp;
	int		cnt{};

	if (!(f = fopen("/run/utmp", "r")))
		throw(std::logic_error("/run/utmp no such file, or i can't open it"));
	while (fread(&tmp, sizeof(tmp), 1, f))
		++cnt;
	fclose(f);
	return (cnt);
}

void	display_gen_info(SysInfo::GenDB &db)
{
	char	buff[9];
	time_t	tloc;
	struct tm *tmp{};

	tloc = time(NULL);
	tmp = localtime(&tloc);
	if (!tmp)
		throw (std::logic_error("Can't get time"));
	if (!strftime(buff, sizeof(buff), "%T", tmp))
		throw (std::logic_error("strftime goes down"));

	struct sysinfo si;

	sysinfo(&si);
	long int	hours = si.uptime / 3600;
	long int	minutes = (si.uptime % 3600) / 60;
	std::ifstream	favg("/proc/loadavg");
	std::vector<std::string> lavg_data{std::istream_iterator<std::string>(favg),
		std::istream_iterator<std::string>()};
	printw("the_top - %s up %ld:%ld, %s users, load average: %s, %s, %s\n",
			buff,
			hours,
			minutes,
			std::to_string(get_users()).c_str(),
			lavg_data[0].c_str(),
			lavg_data[1].c_str(),
			lavg_data[2].c_str());
	mvprintw(1, 0, "%5.5s %-9.9s %2.2s %3.3s %7.7s %7.7s %7.7s %1.1s %4.4s %4.4s %9.9s %.7s\n",
			"PID",
			"USER",
			"PR",
			"NI",
			"VIRT",
			"RES",
			"SHR",
			"S",
			"%CPU",
			"%MEM",
			"TIME+",
			"COMMAND");
}

void	display_proc_info(SysInfo::ProcessesDB &db)
{
	move(2, 0);
	for (auto &proc : db)
	{
		printw("%5.5s %-9.9s %2.2s %3.3s %7.7s %7.7s %7.7s %1.1s %4.4s %4.4s %9.7s %-s\n",
				proc["pid"].c_str(),
				proc["user"].c_str(),
				proc["priority"].c_str(),
				proc["nice"].c_str(),
				proc["vmsize"].c_str(),
				proc["res"].c_str(),
				proc["shared"].c_str(),
				proc["state"].c_str(),
				"%CPU",
				proc["mem"].c_str(),
				proc["time+"].c_str(),
				proc["command"].c_str());
	}
}

int		main(void)
{
	SysInfo	si;
	SysInfo::ProcessesDB	proc;
	SysInfo::GenDB			gen;

	initscr();
	start_color();
	curs_set(0);

	while (true)
	{
		proc = si.get_tasks();
		gen = si.get_gen_data();
		display_gen_info(gen);
		display_proc_info(proc);
		refresh();
		sleep(1);
		erase();
	}
	getch();
	endwin();
	return (0);
}
