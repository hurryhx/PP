#include<getopt.h>

#include<gtkmm/application.h>
#include<gtkmm/window.h>

#include"game2.hh"

using namespace Glib;
using namespace Gtk;

int main(int argc, char* argv[]){
	Glib::RefPtr<Application> app = Application::create(argc, argv, "Game");
	Game game;
	game.set_size(1024, 700);
	game.set_fps(60);
	game.set_nlayer(7);
	game.start();
	return app->run(game);
	return 0;
}
