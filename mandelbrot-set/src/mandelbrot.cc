#include"mandelbrot.hh"

#include<cmath>
#include<cstdlib>

Mandelbrot::Mandelbrot(const Setting &setting):m_setting(setting){
	int color = 8192;
	paint_colorful = new Paint(color);
}

Mandelbrot::~Mandelbrot(){
	delete paint_colorful;
}

void Mandelbrot::render(RenderTask &task)
{
	static const ColorRGB black(0, 0, 0),
				 white(1, 1, 1);
	Complex &c = task.num;

	double x = c.real, y = c.imag;

	int niter = m_setting.max_iter_n;
	for (double x_sqr, y_sqr;
			((x_sqr = x*x) + (y_sqr = y*y)) <= 4 && niter;niter--){
		y = 2*x*y+c.imag;
		x = x_sqr-y_sqr+c.real;
	}
	if (task.cpixel == NULL || task.gpixel == NULL)
		return;
	if (x*x+y*y > 4){

		double dist = (m_setting.max_iter_n - niter) - log(log(x*x+y*y))/M_LN2;
		*task.gpixel = ColorRGB((sin(0.08*dist)+1)*0.5);
		*task.cpixel = ColorRGB(
				dist/m_setting.max_iter_n,
				(cos(0.03*dist)+1)*0.5,
				(sin(0.03*dist)+1)*0.5
				);

	}
	else{
		*task.gpixel = black;
		*task.cpixel = black;
	}
}
