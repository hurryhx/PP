
#include "bsp.hh"

#include <cmath>
#include <algorithm>
#include <cstring>
#include <cassert>

using namespace std;

#define For(i, n) for (int i = 0; i < (n); i ++)

static int Rand(int max){
	return (int)(rand() / (RAND_MAX + 1.0) * max);
}

static int * alloc_ordered_array(int n)
{
	int *ret = new int[n];
	For(i, n) ret[i] = i;
	return ret;
}


void BSPTree::release(Node *root)
{
	if (!root)
		return;
	release(root->ch[0]);
	release(root->ch[1]);
	delete root;
}

void BSPTree::init(const NBodyConfig &conf)
{
	release(root);

	m_nbody = conf.nbody;
	body = conf.body;
	m_dep_max = log((double)m_nbody) / log(2.0) * 4;

	boundary.min_coord = conf.domain.min_coord;
	boundary.max_coord = conf.domain.max_coord;

	//memcpy(m_body, conf.body, sizeof(Body) * m_nbody);

	int *ids = alloc_ordered_array(this->m_nbody);
	root = build_tree(ids, m_nbody, 0);
	delete [] ids;
}

static double cube(const double &x) { return x * x * x; }

template<typename T>
static void checkmin(T &a, T &b){
	if (b < a) a = b;
}
template<typename T>
static void checkmax(T &a, T &b){
	if (b > a) a = b;
}

AABox BSPTree::construct(int *ids, int n)
{
	AABox box;
	box.reset();
	for (int i = 0; i < n; i ++)
	{
		int id = ids[i];
		for (int j = 0; j < 3; j ++)
		{
			if (box.min_coord.coord(j) > body[id].pos.coord(j) - body[id].r)
				box.min_coord.coord(j) = body[id].pos.coord(j) - body[id].r;
			if (box.max_coord.coord(j) < body[id].pos.coord(j) + body[id].r)
				box.max_coord.coord(j) = body[id].pos.coord(j) + body[id].r;
		}
	}

	return box;
}

int BSPTree::find_middle(int *ids, int n, int cid)
{
	//std::sort(ids, ids + n, CmpBody(body, cid));
	//return n / 2;

	int left = 0, right = n;

	int *buf = new int[n];
	int k = (n + 1)/ 2;

	while (left + 1 != right)
	{
		int mid = (left + right) >> 1;
		Body &b = body[ids[mid]];
		double bm = b.pos.coord(cid); // benchmark
		int head = 0, tail = right - left;
		for (int i = left; i < right; i ++)
			if (body[ids[i]].pos.coord(cid) < bm)
				buf[head ++] = ids[i];
			else buf[-- tail] = ids[i];
		while (tail < n && body[buf[tail]].pos.coord(cid) == body[buf[head]].pos.coord(cid))
			tail ++;
		memcpy(ids + left, buf, sizeof(int) * (right - left));
		if (k < head)
			right = left + head;
		else if (k > tail)
			left = left + tail, k -= tail;
		else
		{
			assert(left <= (n + 1) / 2 && (n + 1) / 2 < right);
			return (n + 1) / 2;
		}
	}

	delete buf;

	return right;
}


void BSPTree::check()
{
	return ;
	for (int i = 0; i < m_nbody; i ++)
	{
		assert(!isnan(body[i].pos.x));
		assert(!isnan(body[i].pos.y));
		assert(!isnan(body[i].pos.z));
	}
}

BSPTree::Node *BSPTree::build_tree(int *ids, int nbody, int dep)
{
	check();
	if (nbody == 0)
		return NULL;

	Node *root = new Node();
	root->dep = dep;
	root->box = construct(ids, nbody);
	root->n = nbody;

	root->cm = Position(0, 0, 0);
	root->mass = 0;
	For(i, nbody)
	{
		int id = ids[i];
		root->cm += body[id].pos * body[id].mass;
		root->mass += body[id].mass;
	}
	root->cm /= root->mass;

	if (nbody == 1)
	{
		root->id = *ids;
		return root;
	}

	root->plane.axis = Rand(3);

	int mid = find_middle(ids, nbody, root->plane.axis);
	assert(mid != nbody);
	root->plane.coord = (body[ids[mid]].pos[root->plane.axis] +
			body[ids[mid - 1]].pos[root->plane.axis]) / 2;

	root->ch[0] = build_tree(ids, mid, dep + 1);
	root->ch[1] = build_tree(ids + mid, nbody - mid, dep + 1);
	return root;
}

Collision BSPTree::collision_detect(int id, double dtime)
{
	check();
	Position pos = body[id].pos + body[id].v * dtime;
	double time = REAL_MAX;
	int axis, mag;
	Collision col;
	col.item0 = id;
	col.time = REAL_MAX - 1;
	if (boundary.collide_info(body[id], time, axis, mag))
		if (time > 0 && time < dtime)
		{
			col.item1 = -1;
			col.time = time;
			col.axis = axis;
			col.mag = mag;
		}

	Collision tcol = do_col_dtct(root, id, dtime);
	if (tcol.time > 0 && tcol.time < col.time)
		col = tcol;
	if (col.time > dtime || isinf(col.time))
	{
		col.time = -1;
		col.item1 = -1;
	}
	assert(!isinf(col.time));
	return col;
}

Collision BSPTree::do_col_dtct(Node *root, int id, double dtime)
{
	assert(body[id].id == id);
	Body &b = body[id];
	if (root->is_leaf())
		return body_clsdtct(id, root->id, dtime);

	if (!root->box.enhance(b.r).collide(b.pos, b.v, dtime))
		return Collision(-1, -1, -1);

	int chd = -1;
	if (root->ch[0] == NULL) chd = 1;
	else if (root->ch[1] == NULL) chd = 0;
	else if (b.pos[root->plane.axis] < root->plane.coord) chd = 0;
	else chd = 1;

	Collision ret = do_col_dtct(root->ch[chd], id, dtime);
	if (ret.item0 != -1)
		return ret;
	return do_col_dtct(root->ch[!chd], id, dtime);
}

static bool solve_quadratic_equation(const double &a, const double &b, const double &c,
		double &root0, double &root1)
{
	if (fabs(a) < EPS)
		return false;
	double delta = b * b - 4 * a * c;
	if (delta >= 0)
	{
		double sq = sqrt(delta),
			   d = 1 / (2 * a);
		root0 = (-b - sq) * d;
		root1 = (-b + sq) * d;
		return true;
	}
	return false;
}

Collision BSPTree::body_clsdtct(int item0, int item1, double dtime)
{
	if (item0 == item1)
		return Collision(-1, -1, -1);
	Body &b0 = body[item0], &b1 = body[item1];
	Position AB = b1.pos - b0.pos,
		   vab = b1.v - b0.v;
	double rab = (b0.r + b1.r);
	double a = vab.dot(vab),
		   b = 2 * vab.dot(AB),
		   c = AB.dot(AB) - rab * rab;

	double t0, t1, t;
	if (!solve_quadratic_equation(a, b, c, t0, t1))
		return Collision(-1, -1, -1);
	t = t0;
	if (t < 0) t = t1;
	if (t < 0 || t > dtime) return Collision(-1, -1, -1);
	if (t > EPS)
		t -= EPS / 2;
	return Collision(item0, item1, t);
}

void BSPTree::calc_vel(int id, const double &threshold, const double &dtime)
{
	Body &b = body[id];
	Position a = calc_acc(root, id, threshold);
	b.v += a * dtime;
	int asdf = 0;
}

Position BSPTree::calc_acc(Node *root, int id, double threshold)
{
	if (root == NULL)
		return Position(0, 0, 0);
	assert(!isnan(root->cm.x));
	Body &b = body[id];

	double cube_len = cube((root->cm - b.pos).length());
	double t = root->volume() / cube_len;
	if (root->is_leaf() || root->volume() / cube_len < threshold)
		return calc_single_acc(b, root);
	return calc_acc(root->ch[0], id, threshold)
		+ calc_acc(root->ch[1], id, threshold);
}

Position BSPTree::calc_single_acc(const Body &a, const Node *b)
{
	if (b->is_leaf() && b->id == a.id)
		return Position(0, 0, 0);

	Position dir = b->cm - a.pos;
	double dist_sqr = dir.lengthsqr();
	double magnitude = G * b->mass / dist_sqr;

	Position ret = dir / sqrt(dist_sqr) * magnitude;
	if (dir.lengthsqr() < a.r * a.r)
		return ret*(-1);

	return ret;
}

/**
 * vim: syntax=cpp11 foldmethod=marker
 */

