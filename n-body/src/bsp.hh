
#ifndef __BSP__
#define __BSP__

#include"nbody.hh"
#include"AABox.hh"
#include"engine.hh"

class BSPTree : public Engine {
	public:
		virtual void init(const NBodyConfig &conf);
		virtual Collision collision_detect(int id, double dtime);
		virtual void calc_vel(int id, const double &threshold, const double &dtime);

		BSPTree(){root = NULL;}
		~BSPTree(){}

		struct Plane{
			int axis;
			double coord;
		};
		struct Node{
			Node *ch[2];
			int dep;
			Node(){ch[0] = ch[1] = NULL;}
			bool is_leaf() const {return ch[0] == NULL && ch[1] == NULL;}
			inline double volume() const {return box.volume();}
			Plane plane;

			AABox box;
			int id, n;
			Position cm;
			double mass;
		};
		void release (Node *root);
		AABox construct(int *ids, int n);

		Node *root;
		int m_nbody;
		int m_dep_max;
		Body *body;

		struct CmpBody{
			Body *body;
			int cid;
			CmpBody(Body *body, int cid):body(body), cid(cid){}
			bool operator () (int i, int j){
				return body[i].pos[cid] < body[j].pos[cid];
			}
		};

		AABox boundary;
		int find_middle(int *ids, int n, int cid);
		Node *build_tree(int *ids, int nbody, int dep);
		Collision do_col_dtct(Node *root, int id, double dtime);
		Collision body_clsdtct(int item0, int item1, double dtime);
		Position calc_acc(Node *root, int id, double threshold);

		static Position calc_single_acc(const Body &a, const Node *b);
		void check();
};

#endif
