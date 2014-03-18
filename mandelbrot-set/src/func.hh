

#ifndef __FUNC__
#define __FUNC__

#include"comp.hh"
#include"image.hh"

#define PM_GRAY 0
#define PM_COLORFUL 1

struct RenderTask{
	Complex num;
	int niter;
	ColorRGB *gpixel;
	ColorRGB *cpixel;
	RenderTask(){
		num = Complex(0, 0);
	}
};

class Func{
	public:
		bool iscolor;
		virtual void paint_mode(bool color){
			iscolor = color;
		}

		virtual void render(RenderTask &task)=0;
		virtual ~Func(){}
};

#endif
