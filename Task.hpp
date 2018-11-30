#ifndef TASK_HPP
# define TASK_HPP

# include <string>

// replace by a map ?
class	Task
{
	public:
		Task() = default;
		~Task() = default;
		void		set_pid(std::string const &pid);
		void		set_name(std::string const &name);
		void		set_state(std::string const &state);
		void		set_virt(std::string const &state);
		void		set_nice(std::string const &nice);
		void		set_priority(std::string const &priority);
		void		set_vmsize(std::string const &vmsize);
		void		set_shr(std::string const &shr);
		void		set_res(std::string const &res);
		void		set_user(std::string const &user);
		void		set_timep(std::string const &timep);
		void		set_mem(std::string const &mem);
		std::string	get_pid() const;
		std::string	get_name() const;
		std::string	get_state() const;
		std::string	get_virt() const;
		std::string	get_nice() const;
		std::string	get_priority() const;
		std::string	get_vmsize() const;
		std::string	get_shr() const;
		std::string	get_res() const;
		std::string	get_user() const;
		std::string	get_timep() const;
		std::string	get_mem() const;
	private:
		std::string	_pid;
		std::string	_name;
		std::string	_state;
		std::string	_virt;
		std::string	_nice;
		std::string	_priority;
		std::string	_vmsize;
		std::string	_shr;
		std::string	_res;
		std::string	_user;
		std::string	_timep;
		std::string	_mem;
};

#endif
