#include "Task.hpp"

void	Task::set_pid(std::string const &pid)
{
	_pid = pid;
}

std::string	Task::get_pid() const
{
	return (_pid);
}

void	Task::set_name(std::string const &name)
{
	_name = name;
}

std::string	Task::get_name() const
{
	return (_name);
}

void	Task::set_state(std::string const &state)
{
	_state = state;
}

std::string	Task::get_state() const
{
	return (_state);
}

void	Task::set_virt(std::string const &virt)
{
	_virt = virt;
}

std::string	Task::get_virt() const
{
	return (_virt);
}

void	Task::set_nice(std::string const &nice)
{
	_nice = nice;
}

std::string	Task::get_nice() const
{
	return (_nice);
}

void	Task::set_priority(std::string const &priority)
{
	_priority = priority;
}

std::string	Task::get_priority() const
{
	return (_priority);
}

void	Task::set_vmsize(std::string const &vmsize)
{
	_vmsize = vmsize;
}

std::string	Task::get_vmsize() const
{
	return (_vmsize);
}

void	Task::set_shr(std::string const &shr)
{
	_shr = shr;
}

std::string	Task::get_shr() const
{
	return (_shr);
}

void	Task::set_res(std::string const &res)
{
	_res = res;
}

std::string	Task::get_res() const
{
	return (_res);
}
