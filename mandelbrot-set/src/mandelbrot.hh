
#define TERM_FLAG_NITER 1
#define TERM_EPS_DEFAULT 1e-15
#define TERM_ITER_DEFAULT 200

#include"paint.hh"
#include"func.hh"

struct Setting{
	int term_flag;
	int max_iter_n;
	double eps;
	Setting(int term_flag, int max_iter_n = TERM_ITER_DEFAULT, double eps = TERM_EPS_DEFAULT):term_flag(term_flag), max_iter_n(max_iter_n), eps(eps){}
};

class Mandelbrot:public Func{

	public:

		Setting m_setting;
		Paint *paint_colorful;

		Mandelbrot(const Setting &setting = Setting(TERM_FLAG_NITER));
		virtual ~Mandelbrot();
		virtual void render(RenderTask &task);
};
