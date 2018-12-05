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
#include <algorithm>

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

void	display_gen_info(SysInfo::GenDB &db, SysInfo::ProcessesDB &pdb)
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
	mvprintw(0, 0, "the_top - %s up %2ld:%ld, %2.2s user,  load average: %s, %s, %s\n",
			buff,
			hours,
			minutes,
			std::to_string(get_users()).c_str(),
			lavg_data[0].c_str(),
			lavg_data[1].c_str(),
			lavg_data[2].c_str());
	int		total = pdb.size();
	int		running = std::count_if(pdb.begin(), pdb.end(), [](SysInfo::ProcessDB &t){ return (t["state"] == "R");});
	int		sleeping = std::count_if(pdb.begin(), pdb.end(), [](SysInfo::ProcessDB &t){ return (t["state"] == "S");});
	int		stopped = std::count_if(pdb.begin(), pdb.end(), [](SysInfo::ProcessDB &t){ return (t["state"] == "T");});
	int		zombie = std::count_if(pdb.begin(), pdb.end(), [](SysInfo::ProcessDB &t){ return (t["state"] == "Z");});
	mvprintw(1, 0, "Tasks: %3d total, %3d running, %3d sleeping, %3d stopped, %3d zombie\n",
			total,
			running,
			sleeping,
			stopped,
			zombie);
	mvprintw(2, 0, "%%Cpu(s): %s us, %s\n",
			db["pus"].c_str(),
			db["psy"].c_str());
	std::ifstream	fmi("/proc/meminfo");
	std::vector<std::string> mi_data{std::istream_iterator<std::string>(fmi),
		std::istream_iterator<std::string>()};
	unsigned long int	buffers{};
	unsigned long int	ttl{std::stoul(mi_data[1])};
	unsigned long int	free{std::stoul(mi_data[4])};
	unsigned long int	used{};
	buffers = std::stoul(mi_data[10]) + std::stoul(mi_data[13]) + std::stoul(mi_data[67]);
	used = ttl - free - buffers;
	mvprintw(3, 0, "KiB Mem : %8.8s total, %8.8s free, %8.8s used, %8.8s buff/cache\n",
			mi_data[1].c_str(),
			mi_data[4].c_str(),
			std::to_string(used).c_str(),
			std::to_string(buffers).c_str());

	mvprintw(4, 0, "KiB Swap: %8.8s total\n",
			);
	mvprintw(4, 0, "%5.5s %-9.9s %2.2s %3.3s %7.7s %7.7s %7.7s %1.1s %4.4s %4.4s %9.9s %.7s\n",
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
	move(5, 0);
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
	unsigned long int						prev{};
	unsigned long int						prev2{};

	initscr();
	start_color();
	curs_set(0);

	while (true)
	{
		proc = si.get_tasks();
		gen = si.get_gen_data();
		gen["pus"] = std::to_string((static_cast<double>(std::stoul(gen["us"]) - prev) / prev) * 100.0);
		gen["psy"] = std::to_string((static_cast<double>(std::stoul(gen["sy"]) - prev2) / prev2) * 100.0);
		display_gen_info(gen, proc);
		display_proc_info(proc);
		refresh();
		sleep(1);
		erase();
		prev = std::stoul(gen["us"]);
		prev2 = std::stoul(gen["sy"]);
	}
	getch();
	endwin();
	return (0);
}
