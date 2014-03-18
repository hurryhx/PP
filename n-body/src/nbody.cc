#include"body.hh"
#include"nbody.hh"
#include<vector>
#include<cstdio>
#include<cstdlib>
#include<string.h>


static double Rand(double max){
	return max * (rand() / (RAND_MAX + 1.0));
}

static double RandRange(double low, double high){
	return low + Rand(high - low);
}

static ColorRGB RandColor(){
	return ColorRGB(Rand(1), Rand(1), Rand(1));
}

void NBodyConfig::init(const NBody &domain, int n){

	Position center = (domain.min_coord + domain.max_coord) / 2;
	nbody = n, this->domain = domain;
	body = new Body[n];

	body[0].id = 0;
	body[0].pos = center;
	body[0].v = Position(10, 30, 0);
	body[0].mass = 100000;
	body[0].r = 30;
	body[0].color = ColorRGB(0, 0, 0);

	double R = 200, V = 150;
	int start = 1;
	for (int i = start; i < n; i++){
		double angle = 2 * M_PI / (n-start)*i;
		body[i].pos = center + Position(R*cos(angle), R*sin(angle), 0);
		body[i].v = Position(V*sin(angle), V*cos(angle), 0);
		body[i].mass = i*5;
		body[i].r = pow(body[i].mass, 1/3.0);
		body[i].color = RandColor();
		body[i].id = i;
	}
}

void NBodyConfig::readfile(const char* filename){
	FILE *rfile;
	if (!strcmp(filename, "-"))
		rfile = stdin;
	else rfile = fopen(filename, "r");
	std::vector<Body> bvec;
	Body b;
	nbody = 0;
	while(fscanf(rfile, "%lf %lf %lf %lf %lf %lf %lf %lf", &b.pos.x, &b.pos.y, &b.pos.z, &b.v.x, &b.v.y, &b.v.z, &b.mass, &b.r) == 8){
		b.color = RandColor();
		b.id = nbody;
		bvec.push_back(b);
		nbody++;
	}
	body = new Body[nbody];
	for (int i = 0; i < bvec.size(); i++)
		body[i] = bvec[i];
	if (rfile != stdin)
		fclose(rfile);
}
