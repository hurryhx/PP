#ifndef __GTK__
#define __GTK__

#include<gtk/gtk.h>

#include<getopt.h>
#include<unistd.h>

#include<cstring>
#include<algorithm>
#include<cstdlib>
#include<cmath>
#include<queue>
#include<cassert>

#include"nbody.hh"
#include"bsp.hh"
#include"simple.hh"
#include"collision.hh"
#include"openmp.hh"
#include"pthread.hh"
#include"mpi.hh"
#include"timer.hh"

const double FPS = 1000;
const char *progname;
bool show_window = true;


struct Camera
{
	Position sight_pos;
	Position view_dir, up_dir;
	double frustum_dist;
	double fwidth, fheight; // configuration of the frustum
};

struct RenderConfig
{
	NBodyConfig nbody;

	Worker *worker;
	Camera camera;

	int wwidth, wheight; // window width and height

	int width() const { return wwidth; }
	int height() const { return wheight; }

	void set_worker(Worker *worker)
	{ this->worker = worker; }

	bool advance(double dtime)
	{
		if (fabs(dtime) < EPS)
			return true;
//		assert(dtime > 0);
		return worker->advance(nbody, dtime);
	}

	void init() {}

} rconf;
Timer timer;

long long frame_count;
double runtime = 1e100;
Timer gtimer;

FpsCounter fps;

bool rendered = false;

void exit_on_timeout()
{
	if (rendered)
	{
		double rtime = gtimer.end() / 1000.0;
		printf("worker: %s\n", rconf.worker->name().c_str());
		printf("nworker: %d\n", rconf.worker->get_nworker());
		printf("running time: %lf\n", rtime);
		printf("total frames: %lld\n", frame_count);
		printf("average fps: %lf\n", frame_count / rtime);
	}
	delete rconf.worker;

	exit(0);
}

bool cmpZ(const Body &a, const Body &b)
{
	return a.pos.z + a.r > b.pos.z + b.r;
}
static gboolean delete_event(
		GtkWidget *,
		GdkEvent *,
		gpointer )
{
	return FALSE;
}

static void destroy(GtkWidget *, gpointer )
{
	gtk_main_quit();
}

static gboolean cb_timeout(GtkWidget *widget)
{
	if (widget->window == NULL)
		return FALSE;

	gtk_widget_queue_draw_area(widget, 0, 0, widget->allocation.width, widget->allocation.height);
	return TRUE;
}
int fcnt = 0;
static gboolean da_expose_callback(
		GtkWidget *widget,
		GdkEventExpose *,
		gpointer )
{
	if (timer.end() > EPS)
	{
		fps.count();
		frame_count ++;
		Timer tengine;
		tengine.begin();
		rconf.advance(timer.end() / 1000.0);
		//log_printf("engine time: %llums\n", tengine.end());
		timer.begin();
		if (gtimer.end() > runtime * 1000)
			exit_on_timeout();
	}

	if (!show_window)
		return FALSE;
	//return FALSE;
	cairo_t *cr = gdk_cairo_create(widget->window);

	// clear scene
	cairo_save (cr);
	cairo_new_path(cr);
	cairo_arc(cr, 0, 0, 900, 0, 2*M_PI);
	cairo_close_path(cr);
	cairo_set_source_rgb(cr, 255.0, 255.0, 255.0);
	cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
	cairo_fill_preserve(cr);
	cairo_stroke(cr);
	cairo_paint (cr);
	cairo_restore (cr);

	NBodyConfig &nbody = rconf.nbody;
	Body *body = new Body[nbody.nbody];
	memcpy(body, nbody.body, sizeof(Body) * nbody.nbody);

	std::sort(body, body + nbody.nbody, cmpZ);
	Camera &camera = rconf.camera;

	cairo_save(cr);
	cairo_new_path(cr);
	cairo_arc(cr, 0.0, 0.0, 10000.0, 0, 2*M_PI);
	cairo_close_path(cr);
	cairo_set_source_rgb(cr, 255.0, 255.0, 255.0);
	cairo_fill_preserve(cr);
	cairo_stroke(cr);
	cairo_restore(cr);


	NBody &domain = rconf.nbody.domain;
	for (int i = 0; i < nbody.nbody; i ++)
	{
		Body &b = body[i];
		int x = b.pos.x / domain.length(0) * rconf.width(),
			y = b.pos.y / domain.length(1) * rconf.height();

		cairo_save(cr);

		cairo_new_path(cr);
		cairo_arc(cr, x, y, b.r, 0, 2 * M_PI);
		cairo_close_path(cr);

		ColorRGB &col = body[i].color;

		cairo_set_source_rgb(cr, col.r, col.g, col.b);
		cairo_fill_preserve(cr);
		cairo_stroke(cr);

		cairo_restore(cr);
	}


	delete [] body;
	cairo_set_source_rgb(cr, 0.0, 100.5, 0.5);;
	cairo_select_font_face(cr, "simsun",
			CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 20);
	cairo_move_to(cr, 420, 30);

	char sfps[20];
	snprintf(sfps, 20, "FPS: %.2lf", fps.fps());
	if (fcnt == 1000) {}
	cairo_show_text(cr, sfps);

	cairo_destroy(cr);
	return FALSE;
}

void show_init(int argc, char *argv[])
{
	gtk_init(&argc, &argv);
}

void show()
{
	GtkWidget *window;
	int border_width = 0;

	int window_width = rconf.width() + border_width * 2,
		window_height = rconf.height() + border_width * 2;
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(window), border_width);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), window_width, window_height);

	//gtk_widget_set_app_paintable(window, TRUE);
	gtk_widget_set_size_request(window, window_width, window_height);

	GtkWidget *da = gtk_drawing_area_new();
	gtk_widget_set_size_request(da, rconf.width(), rconf.height());

	g_signal_connect(window, "delete-event",
			G_CALLBACK(delete_event), NULL);
	g_signal_connect(window, "destroy",
			G_CALLBACK(destroy), NULL);

	/*
	gtk_widget_add_events(da, GDK_BUTTON_PRESS_MASK);
	g_signal_connect(da, "button-press-event",
			G_CALLBACK(cb_clicked), NULL);
	*/

	g_signal_connect(da, "expose_event",
			G_CALLBACK(da_expose_callback), NULL);

	g_timeout_add(1000 / FPS, (GSourceFunc)cb_timeout, da);

	gtk_container_add(GTK_CONTAINER(window), da);
	gtk_widget_show_all(window);

	timer.begin();
	gtk_main();
}








int str2num(const std::string &str)
{
	int ret = 0, sign = 1, start = 0;
	if (str[0] == '-')
		sign = -1, start = 1;
	for (size_t i = start; i < str.length(); i ++)
		ret = ret * 10 + str[i] - '0';
	return ret * sign;
}

double str2real(const std::string &str)
{
	double ret;
	sscanf(str.c_str(), "%lf", &ret);
	return ret;
}
#endif
