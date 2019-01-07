#ifndef METERSWINDOW_HPP
# define METERSWINDOW_HPP

# include "IVisual.hpp"
# include "Window.hpp"

class MetersWindow : public Window
{
	public:
		MetersWindow(int nlines, int ncols, int begin_y, int begin_x);
		void	draw(IVisual::Meminfo const &memi, IVisual::Cpu_usage const &usage);
	private:
		void				_display_cpu_bar(IVisual::Cpu_usage const &usage);
		void				_display_mem_bar(IVisual::Meminfo const &memi);
		void				_display_swap_bar(IVisual::Meminfo const &memi);
		void				_display_meter(int cp, int times);
};

#endif
