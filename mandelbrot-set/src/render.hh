

#ifndef __RENDER__
#define __RENDER__

#include<cstdlib>
#include<cstdio>
#include"rect.hh"
#include"func.hh"
#include"image.hh"

#define REAL_T_FMT "lf"

struct GraphRenderConf{
	int width; int height;
	Rect domain;
	int nworker;
	bool is_res;

	GraphRenderConf(){}
	GraphRenderConf(int width, int height, const Rect &domain, int nworker, bool is_res = true) : width(width), height(height), domain(domain), nworker(nworker), is_res (is_res){}
	void show(){
		fprintf(stderr, "image size: %dx%d\n", width, height);
		fprintf(stderr, "domain: (%.18" REAL_T_FMT ",%.18" REAL_T_FMT ")\n        [%.18" REAL_T_FMT ",%.18" REAL_T_FMT "]\n", domain.x, domain.y, domain.width, domain.height);
		fprintf(stderr, "nworker: %d\n", nworker);
	}


};



struct GraphRenderRes{
	Image *rgb, *gray;
	GraphRenderRes():rgb(NULL), gray(NULL){}
	GraphRenderRes(int width, int height){
		rgb = new Image(width, height);
		gray = new Image(width, height);
	}
	~GraphRenderRes(){
		if (rgb)
			delete rgb;
		if (gray)
			delete gray;
	}

};

class GraphRender{
	public:
		virtual GraphRenderRes *render(Func *func, const GraphRenderConf &conf)=0;
		virtual ~GraphRender(){}
};
#endif
