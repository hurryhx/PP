
#include<gtkmm/drawingarea.h>
#include<gdkmm/pixbuf.h>
#include<gtkmm/image.h>
#include<gtkmm/fixed.h>

#ifndef __CANVAS__
#define __CANVAS__

class Game;

struct Pos{
	int x, y;
	Pos(int setx, int sety) : x(setx), y(sety){}
	Pos(){}
};

class Image{
	protected:
		Gtk::Image m_img_protect;
		int m_width, m_height;
		int m_swidth, m_sheight;
	public:
		Image():m_width(-1), m_height(-1), m_swidth(-1), m_sheight(-1){};
		int width(){return m_width;}
		int height(){return m_height;}
		int swidth(){return m_swidth;}
		int sheight(){return m_sheight;}
		Gtk::Image m_img;
		void set_size(int width, int height);
		void set_img(char* img_path);
		void save_size(){
			m_swidth = m_width; m_sheight = m_height;
		};
};

class Canvas : public Gtk::Fixed{
	protected:
		double m_width_ratio, m_height_ratio;
		Game *m_game;
	public:
		Canvas(Game *game):m_width_ratio(1), m_height_ratio(1), m_game(game){}
		Canvas():m_width_ratio(1), m_height_ratio(1){}

		int width() const{return get_width();}
		int height() const{return get_height();}

		void set_img(Image *img, Pos pos);
		void move_img(Image *img, Pos pos);
		void set_scale(double r_width, double r_height){
			m_width_ratio = r_width;
			m_height_ratio = r_height;
		};

};

#endif
