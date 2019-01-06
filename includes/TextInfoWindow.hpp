#ifndef TEXTINFOWINDOW_HPP
# define TEXTINFOWINDOW_HPP

# include "Window.hpp"
# include "IVisual.hpp"

class TextInfoWindow : public Window
{
	public:
		TextInfoWindow(int nlines, int ncols, int begin_y, int begin_x);
		void	draw(int threads, int tasks, int r,
					IVisual::Load_avg const &load_avg, long int uptime);
};

#endif
