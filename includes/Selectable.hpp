#ifndef SELECTABLE_HPP
# define SELECTABLE_HPP

class Selectable
{
	protected:
		void			_handle_up_vp_border();
		void 			_handle_down_vp_border(int size);
		unsigned int	_vp_start{}; // vp -> view point
		unsigned int	_vp_end{};
		unsigned int	_selected{};
};

#endif
