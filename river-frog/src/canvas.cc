
#include<gtkmm.h>
#include"canvas.hh"


void Image::set_size(int width, int height){
	m_width = width, m_height = height;
	Glib::RefPtr<Gdk::Pixbuf> buf = m_img_protect.get_pixbuf()->copy();
	m_img.set(buf->scale_simple(width, height, Gdk::INTERP_HYPER));
}

void Image::set_img(char* img_path)
{
	m_img_protect.set(img_path);
	m_img.set(m_img.get_pixbuf());
}


void Canvas::set_img(Image *img, Pos pos){
	img->set_size(img->swidth() * m_width_ratio, img->sheight() * m_height_ratio);
	put(img->m_img, pos.x * m_width_ratio, pos.y * m_height_ratio);
}

void Canvas::move_img(Image *img, Pos pos){
	img->set_size(img->swidth() * m_width_ratio, img->sheight() * m_height_ratio);

	move(img->m_img, pos.x * m_width_ratio, pos.y * m_height_ratio);
}
