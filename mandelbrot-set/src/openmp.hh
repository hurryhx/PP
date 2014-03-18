

#ifndef __OPENMP__
#define __OPENMP__

#include"render.hh"
#include<cstdio>

class OpenmpRender: public GraphRender{
	public:
		virtual GraphRenderRes *render(Func *func, const GraphRenderConf &conf);
};

#endif
