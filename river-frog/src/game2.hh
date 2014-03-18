#include<vector>
#include<deque>
#include<pthread.h>
#include<gtkmm.h>
#include<gtkmm/window.h>
#include"canvas.hh"
#include"timer.hh"
#include<cstdlib>
#include<cassert>

#define real_t double
#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_LEFT 4
#define KEY_RIGHT 8
#define LAND_START 1
#define LAND_END 1

#define EPS 1e-6
#define WoodImgPath "../res/wood.jpg"
#define DefaultSpeed 400.0
#define F_DefaultWidth 50.0

#ifndef __GAME__
#define __GAME__

class Frog;
class Layer;
class Game;


class Wood : public Image{
	public:
		std::vector<Frog*> m_frog;
		double m_pos;
		double m_speed;
		double m_width, m_height;
		Layer* m_layer;
		Game* m_game;
		double head(){
			return m_pos-m_width/2;
		}
		double tail(){
			return m_pos+m_width/2;
		}


		Wood(double x, double y){
			m_speed = 1.0;
			m_pos = x;
		}
		Wood(Layer *layer, double pos, double width, double speed, char* imgpath, Game *game):m_layer(layer), m_width(width), m_speed(speed), m_pos(pos), m_game(game){
			set_img(imgpath);
		};

		bool add_frog(Frog *frog){
			for (int i = 0; i < m_frog.size(); i++)
				if (m_frog[i] == frog) return false;
			m_frog.push_back(frog);
			return true;
		}

		bool remove_frog(Frog *frog){
			for (int i = 0; i < m_frog.size(); i++)
			{
				if(m_frog[i] == frog){
					m_frog.erase(m_frog.begin()+i);
					return true;
				}
			}
			return false;
		}

		bool update(double time_remain);
		virtual ~Wood(){}
};

class Frog : public Image{
	public:
		Wood* m_wood;
		int m_key_status;
		double m_pos, m_speed;
		int m_layer;
		Game* m_game;

		Frog(double pos, double speed, char* imgpath, Game* game):m_pos(pos), m_speed(speed), m_wood(NULL), m_key_status(0), m_game(game){
			set_img(imgpath);
		}

		void set_pos(double pos){
			m_pos = pos;
		}

		bool on_wood(Wood *wood){
			return m_pos >= wood->head() && m_pos <= wood->tail();
		}

		bool init();

		void update(double time_remain){
			if (m_wood == NULL){ //if not on a wood
				return;
			} // if on a wood
			if (m_key_status & KEY_LEFT) m_pos -= m_speed * time_remain;
			if (m_key_status & KEY_RIGHT) m_pos += m_speed * time_remain;
			if (m_pos < m_wood->head()) m_pos = m_wood->head();
			if (m_pos > m_wood->tail()) m_pos = m_wood->tail();
		//	if (m_key_status & KEY_UP) frog_up();
		//	if (m_key_status & KEY_DOWN) frog_down();

		}

		void reset(){
			if(m_wood) m_wood->remove_frog(this);
			m_wood = NULL;
			m_layer = 0;
			init();

		}

		bool frog_up();
		bool frog_down();


		void up_pre(){m_key_status |= KEY_UP;frog_up();}
		void down_pre(){m_key_status |= KEY_DOWN;frog_down();}
		void left_pre(){m_key_status |= KEY_LEFT;}
		void right_pre(){m_key_status |= KEY_RIGHT;}


		void up_release(){m_key_status &= ~KEY_UP;}
		void down_release(){m_key_status &= ~KEY_DOWN;}
		void left_release(){m_key_status &= ~KEY_LEFT;}
		void right_release(){m_key_status &= ~KEY_RIGHT;}

		virtual ~Frog(){}
};

struct arg_pth{
	Layer *layer;
	double time_remain;
};


class Game : public Gtk::Window {
public:
	Canvas m_canvas;
	std::vector<Wood*> m_woods;
	std::vector<Frog*> m_frog;
	std::vector<Layer*> m_layers;
	std::vector<pthread_t> m_threads;
	std::vector<arg_pth> m_targs;
	Image *m_bg;
	Timer timer;
	int m_nlayer;
	double m_width, m_height;
	double refresh_interval;
	int fps;
	bool multi;
	sigc::connection conn;
	sigc::slot<bool> slot_up;

	Game();
	void set_size(int width, int height){
		m_width = width;
		m_height = height;
	}
	void set_fps(int f){
		fps = f;
	}
	void set_nlayer(int n){
		m_nlayer = n;
	}

	void init();
	bool update();
	void update_canvas();
	Pos get_pos(int layer, double pos_on_layer, double width, double height){

		Pos pos;
		pos.x = pos_on_layer - width/2;

		double layer_height = m_height / (m_nlayer+LAND_START+LAND_END);

		double factor;
		if (layer == 0)
			factor = LAND_START+m_nlayer;
		else if (layer == m_nlayer+1)
			factor = 0;
		else factor = LAND_END+m_nlayer-layer;

		pos.y = layer_height * factor;

		return pos;
	}

	void start();


	double layer_height(){
		return height() / (m_nlayer + LAND_START + LAND_END);
	}

	void insert_wood(int layer, int pos);

	virtual bool on_key_press_event(GdkEventKey *event){
		if (event->keyval == GDK_KEY_Up) m_frog[0]->up_pre();
		if (event->keyval == GDK_KEY_Down) {
			if (m_frog[0]->m_layer == 0) return true;
			m_frog[0]->down_pre();}
		if (event->keyval == GDK_KEY_Left) m_frog[0]->left_pre();
		if (event->keyval == GDK_KEY_Right) m_frog[0]->right_pre();
		if (event->keyval == GDK_KEY_space){
		//	insert_wood(m_frog[0]->m_layer, m_frog[0]->m_pos);
			if (multi == 1) return true;
			Frog *frog = new Frog(m_width/2, DefaultSpeed, "../res/frog1.png", this);
			frog->set_size(F_DefaultWidth, layer_height());
			frog->save_size();
			m_frog.push_back(frog);
			multi = 1;
		}
		if (multi == 1){
			if (event->keyval == GDK_KEY_w) m_frog[1]->up_pre();
			if (event->keyval == GDK_KEY_s) {
				if (m_frog[1]->m_layer == 0) return true;
				m_frog[1]->down_pre();
			}
			if (event->keyval == GDK_KEY_a) m_frog[1]->left_pre();
			if (event->keyval == GDK_KEY_d) m_frog[1]->right_pre();
		}
	}
	virtual bool on_key_release_event(GdkEventKey *event){
		if (event->keyval == GDK_KEY_Left) m_frog[0]->left_release();
		if (event->keyval == GDK_KEY_Right) m_frog[0]->right_release();
		if (multi == 1){
			if (event->keyval == GDK_KEY_a) m_frog[1]->left_release();
			if (event->keyval == GDK_KEY_d) m_frog[1]->right_release();
		}
	}

	int height(){
		int w,h;
		get_size(w, h);
		return h-4;
	}
	int width(){
		int w,h;
		get_size(w, h);
		return w-4;
	}


	virtual ~Game(){
	};

};



class Layer{
	public:
		std::vector<Wood *> m_woods;
		double m_layer_pos;
		double m_layer_width;
		int m_layer_id;

		int Rand(int max){
			return rand() / (RAND_MAX+1.0) * max;
		}

		int RandRange(int min, int max){
			return min+Rand(max-min+1);
		}

		void insert_wood(int pos){
			bool flag = 1;
			int cnt = 0;
			for(int i = 0; i < m_woods.size(); i++){
				Wood* wood = m_woods[i];
				if ((wood->head() < pos-30.0 && wood->tail() > pos-30.0) || (wood->head() < pos+30.0 && wood->tail() > pos+30.0))
					flag = 0;
				if (wood->head() < pos-30.0 && wood->tail() < pos-30.0)
					cnt = i;
			}
			if (flag){
				Wood *wood = new Wood(this, pos, 30.0, 50.0, "../res/wood.png", m_game);
				wood->set_size(30, m_game->layer_height());
				wood->save_size();
				m_woods.insert(m_woods.begin()+cnt, wood);

			}
		}

		double head() {return m_layer_pos - m_layer_width/2;}
		double tail() {return m_layer_pos + m_layer_width/2;}

		Game *m_game;

		Layer(int id, double pos, double width, Game *game):m_layer_id(id), m_layer_pos(pos), m_layer_width(width), m_game(game){}

		void init(int wood_width_min, int wood_width_max, int wood_speed_min, int wood_speed_max, int min_spacing, int max_spacing){
			clear_wood();

			for (real_t pos = m_layer_width * 0.05; pos + wood_width_min - 1 < m_layer_width * 0.95; )
			{

				double width = RandRange(wood_width_min, wood_width_max),
					   speed =	(Rand(2) ? 1 : -1) * RandRange(wood_speed_min, wood_speed_max);

				if (pos + width >= m_layer_width)
					break;

				Wood *wood = new Wood(this, head()+pos+width/2, width, speed, "../res/wood.png", m_game);
				wood->set_size(width, m_game->layer_height());
				wood->save_size();

				if (m_woods.size())
					assert(wood->head() >= m_woods.back()->tail());

				m_woods.push_back(wood);

				pos += width + RandRange(min_spacing, max_spacing);
			}



		}

		void clear_wood(){
			for (int i = 0; i < m_woods.size(); i++)
				delete m_woods[i];
			m_woods.clear();
		};

		void add_wood(Wood *wood){m_woods.push_back(wood);}

		struct Collide{
			double time;
			Wood *wood, *wood_collide;
			Collide(){}
			Collide(double time, Wood *wood, Wood *wood_collide = NULL):time(time), wood(wood), wood_collide(wood_collide){}
		};
		bool update(double time_remain){

			Collide last_col;
			last_col.time = 1000;
			last_col.wood = last_col.wood_collide = NULL;
			do{
				Collide col;

				col.time = 1000;
				col.wood = col.wood_collide = NULL;
				for (size_t i = 0; i < m_woods.size(); i ++)
				{
					Wood *w0 = m_woods[i];
					if (i == 0 && w0->m_speed < 0)
					{
						real_t s = w0->head() - this->head(),
							   v = -w0->m_speed,
							   t = s / v;
						if (t < col.time)
							col = Collide(t, w0);
					}
					if (i == m_woods.size() - 1 && w0->m_speed > 0)
					{
						real_t s = this->tail() - w0->tail(),
							   v = w0->m_speed,
							   t = s / v;
						if (t < col.time)
							col = Collide(t, w0);
					}
					if (i < m_woods.size() - 1)
					{
						Wood *w1 = m_woods[i + 1];
						if (fabs(w0->m_speed - w1->m_speed) > EPS)
						{
							real_t s = w1->head() - w0->tail(),
								   v = w0->m_speed - w1->m_speed,
								   t = s / v;
							if (t > 0 && t < col.time)
								col = Collide(t, w0, w1);
						}
					}
				}

				col.time -= EPS / 2;
				if (col.time > time_remain)
					break;
				time_remain -= col.time;
				for (size_t i = 0; i < m_woods.size(); i ++)
				{
					Wood *wood = m_woods[i];
					wood->update(col.time);
				}

		// calculate speed
				do
				{

					Wood *w0 = col.wood,
						 *w1 = col.wood_collide;
					if (w1 == NULL)
						w0->m_speed *= -1;
					else
					{
						double m0 = w0->m_width, v0 = w0->m_speed,
							   m1 = w1->m_width, v1 = w1->m_speed;
						w0->m_speed = (2*m1*v1+(m0-m1)*v0)/(m0+m1);
						w1->m_speed = (2*m0*v0+(m1-m0)*v1)/(m0+m1);
					}


				} while (0);
				last_col = col;
			} while (time_remain > EPS);

			for (size_t i = 0; i < m_woods.size(); i ++)
			{
				Wood *w0 = m_woods[i];
				w0->update(time_remain);
			}


			return true;
		}

		Wood* find_frog(Frog *frog){
			for (int i = 0; i < m_woods.size(); i++)
			{
				Wood *wood = m_woods[i];
				if (frog->on_wood(wood))
					return wood;
			}
			return NULL;
		}
		virtual ~Layer(){
			clear_wood();
		};


};

//Game *game;
//
#endif
