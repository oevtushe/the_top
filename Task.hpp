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
		void		set_shr(std::string const &vmsize);
		void		set_res(std::string const &vmsize);
		std::string	get_pid() const;
		std::string	get_name() const;
		std::string	get_state() const;
		std::string	get_virt() const;
		std::string	get_nice() const;
		std::string	get_priority() const;
		std::string	get_vmsize() const;
		std::string	get_shr() const;
		std::string	get_res() const;
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
};

#endif
