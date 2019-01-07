#include "Selectable.hpp"

void	Selectable::_handle_up_vp_border()
{
	if (_selected)
		--_selected;
	else
	{
		if (_vp_start)
		{
			--_vp_start;
			--_vp_end;
		}
	}
}

void	Selectable::_handle_down_vp_border(unsigned int size)
{
	if (_selected + _vp_start + 1 >= _vp_end)
	{
		if (_vp_end < size)
		{
			++_vp_end;
			++_vp_start;
		}
	}
	else if (_selected + 1 < size)
		++_selected;
}
