
#include"paint.hh"
#include"comp.hh"
#include"image.hh"
#include"render.hh"
#include"rect.hh"
#include"image.hh"

#include<cstdio>
#include<cstdlib>
#include<string>
#include<string.h>
#include<cairo.h>
#include<gtk/gtk.h>
#include<gdk-pixbuf/gdk-pixbuf.h>


GdkPixbuf *img2pixbuf(Image *img);

#define ST_GRAY 0
#define ST_RGB 1

using namespace std;

struct RenderConfig{
	GraphRender *rdr;
	Func *func;
	GraphRenderConf gc;
	GraphRenderRes *output;
	GdkPixbuf *gpixbuf, *cpixbuf;
	GdkPixbuf *pixbuf;

	int width(){return output->rgb->width;}
	int height(){return output->rgb->width;}

	int show_type;
	void set_show_type(int type){
		pixbuf = cpixbuf;
		show_type = type;
	}

	RenderConfig(){
		memset(this, 0, sizeof(RenderConfig));
		show_type = ST_RGB;
	}

	void unref(){
		if (gpixbuf)
			g_object_unref(gpixbuf);
		if (cpixbuf)
			g_object_unref(cpixbuf);
	}

	~RenderConfig(){
		delete rdr;
		delete func;
		if (output) delete output;
		unref();
	}

	bool render(bool to_pix=false){
		if (output) delete output;
		if (cpixbuf){
			g_object_unref(cpixbuf);
			cpixbuf = NULL;
			pixbuf = cpixbuf;
		}
		if (gpixbuf){
			g_object_unref(gpixbuf);
			gpixbuf = NULL;
			pixbuf = gpixbuf;
		}

		gc.show();

		output = rdr->render(func, gc);

		if (to_pix)
			this->to_pixbuf();

		if (output == NULL)
			return false;
		return true;
	}

	void to_pixbuf(){
		if (output){
			unref();
			cpixbuf = img2pixbuf(output->rgb);
			gpixbuf = img2pixbuf(output->gray);
			pixbuf = cpixbuf;
		}
	}
};


RenderConfig rcfg;

static gboolean delete_event(
		GtkWidget *,
		GdkEvent *,
		gpointer
		){
	return FALSE;
}


static void destroy(GtkWidget *, gpointer){
	gtk_main_quit();
}


GdkPixbuf *img2pixbuf(Image *img){
	GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, img->width, img->height);
	int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
	int rowstride = gdk_pixbuf_get_rowstride(pixbuf);

	guchar *pixel = gdk_pixbuf_get_pixels(pixbuf);

	for (int i = 0; i < img->width; i++){
		for (int j = 0; j < img->height; j++){
			guchar *p = pixel + j *rowstride + i * n_channels;
			ColorRGB *c = img->data + i * img->height + j;
			p[0] = c->r * 255;
			p[1] = c->g * 255;
			p[2] = c->b * 255;
		}
	}
	return pixbuf;
}


static gboolean da_expose_callback(
		GtkWidget *widget,
		GdkEventExpose *,
		gpointer
		){
	GdkPixbuf *pix = rcfg.pixbuf;
	cairo_t *cr = gdk_cairo_create(widget->window);
	gdk_cairo_set_source_pixbuf(cr, pix, 0, 0);
	cairo_paint(cr);
	cairo_destroy(cr);
	return FALSE;
}

static gboolean cb_timeout(GtkWidget *widget){
	if (widget->window == NULL)
		return FALSE;

	gtk_widget_queue_draw_area(widget, 0, 0, widget->allocation.width, widget->allocation.height);
	return TRUE;
}

double zoom_scale = 2.0;

static gboolean cb_clicked(GtkWidget *, GdkEventButton *event, gpointer){
	static const unsigned LEFT_BUTTON = 1,
				 MIDDLE_BUTTON = 2,
				 RIGHT_BUTTON = 3;
	if (event->button == LEFT_BUTTON || event->button == RIGHT_BUTTON || event->button == MIDDLE_BUTTON){
		int cx = event -> x;
		int cy = event -> y;

		GraphRenderConf &gc = rcfg.gc;
		Rect &dm = gc.domain;

		double rx = dm.x + (double)cx / gc.width * dm.width;
		double ry = dm.y + (double)(gc.height - cy) / gc.height * dm.height;
		if (event->button == LEFT_BUTTON)
			dm.width /= zoom_scale, dm.height /= zoom_scale;
		else if (event->button == RIGHT_BUTTON)
			dm.width *= zoom_scale, dm.height *= zoom_scale;

		dm.x = rx - dm.width / 2;
		dm.y = ry - dm.height / 2;

		rcfg.render(true);
	}
	return TRUE;
}



void image_init(int argc, char* argv[]){
	gtk_init(&argc, &argv);
}

void image_show(){
	GtkWidget *window;

	int border_width = 0;
	int window_width = rcfg.width()+border_width*2;
	int window_height = rcfg.height()+border_width*2;
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(window), border_width);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), window_width, window_height);
	gtk_widget_set_size_request(window, window_width, window_height);

	GtkWidget *da = gtk_drawing_area_new();
	gtk_widget_set_size_request(da, rcfg.width(), rcfg.height());

	g_signal_connect(window, "delete-event", G_CALLBACK(delete_event), NULL);
	g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
	gtk_widget_add_events(da, GDK_BUTTON_PRESS_MASK);
	g_signal_connect(da, "button-press-event", G_CALLBACK(cb_clicked), NULL);
	g_signal_connect(da, "expose_event", G_CALLBACK(da_expose_callback), NULL);
	g_timeout_add(1000/30, (GSourceFunc)cb_timeout, da);
	gtk_container_add(GTK_CONTAINER(window), da);
	gtk_widget_show_all(window);

	gtk_main();
}

void img2ppm(Image *image, string filename){
	FILE *fout;
	if (filename == "-")
		fout = stdout;
	else fout = fopen(filename.c_str(), "w");

	fprintf(fout, "P6\n%d %d\n255\n", image->width, image->height);
	unsigned char pixel[3];
	for (int i = 0; i < image->height; i++){
		for (int j = 0; j < image->width; j++){
			ColorRGB *c = image->data + j*image->height + i;
			pixel[0] = c->r*255;
			pixel[1] = c->g*255;
			pixel[2] = c->b*255;
			fwrite(pixel, sizeof(pixel), 1, fout);
		}
	}

	if (fout != stdout)
		fclose(fout);
}

void img2pgm(Image *image, string filename){
	FILE *fout;
	if (filename == "-")
		fout = stdout;
	else fout = fopen(filename.c_str(), "w");

	fprintf(fout, "P5\n%d %d\n225\n", image->width, image->height);
	unsigned char pixel;
	for (int i = 0; i < image->height; i++){
		for (int j = 0; j < image->width; j++){
			ColorRGB *c = image->data + j * image->height + i;
			pixel = c->r*255;
			fwrite(&pixel, sizeof(pixel), 1, fout);
		}
	}

	if (fout != stdout)
		fclose(fout);
}

template<typename T>
void check_lower_bound(T &num, T lower_bound){
	if (num < lower_bound)
		num = lower_bound;
}

int str2num(std::string str){
	int ret = 0, sign = 1, start = 0;
	if (str[0] == '-')
	{	sign = -1; start = 1;}
	for (int i = start; i < str.length(); i++)
		ret = ret*10+str[i]-'0';
	return ret*sign;

}

double str2real(string str){
	double ret;
	sscanf(str.c_str(), "%lf", &ret);
	return ret;
}


