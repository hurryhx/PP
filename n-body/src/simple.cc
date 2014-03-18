
#include"simple.hh"

void SimpleEngine::init(const NBodyConfig &conf){
	this->conf = conf;
	boundary.min_coord = conf.domain.min_coord;
	boundary.max_coord = conf.domain.max_coord;

	body = conf.body;
}

void SimpleEngine::calc_vel(int id, const double &threshold, const double &dtime){
	Position accel(0, 0, 0);
	for (int i = 0; i < conf.nbody; i++){
		if (i != id)
			accel += calc_acc(body[id].pos, body[i].pos, body[i].mass);
	}
	body[id].v += accel * dtime;
}

Collision SimpleEngine::collision_detect(int id, double dtime){
	int axis, mag;
	double time = REAL_MAX;
	Collision col;
	col.time = REAL_MAX;
	col.item0 = id;
	if (boundary.collide_info(body[id], time, axis, mag)){
		if (time < dtime){
			col.item1 = -1;
			col.time = time;
			col.axis = axis;
			col.mag = mag;
		}
	}
	for (int i = 0; i < conf.nbody; i++) //make col least
		if(i != id){
			Collision tcol = body_clsdtct(body[id], body[i], dtime);
			if (tcol.time < dtime && tcol.time > 0 && tcol.time < col.time)
				col = tcol;
		}

	if (col.time > dtime){
		col.time = -1; col.item1 = -1;
	}
	return col;
}
