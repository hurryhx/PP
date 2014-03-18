#include"canvas.hh"
#include"game2.hh"

#define Default_Width 1024
#define Default_Height 700
#define WoodDefaultWidth 300
#define WoodDefaultSpeed 60

Game game;

static void *call_threads(void* arg){
	arg_pth *parg = (arg_pth*)arg;
	parg->layer->update(parg->time_remain);
	pthread_exit(NULL);
}



Game::Game():m_canvas(this){
	multi = 1;
	set_size(Default_Width, Default_Height);
}



void Game::init(){
	m_bg = new Image();
	m_bg->set_img("../res/bg.png");
	m_bg->set_size((int)m_width, (int)m_height);
	m_bg->save_size();

	Frog *frog = new Frog(m_width / 2, DefaultSpeed, "../res/frog2.png", this);
	frog->set_size(F_DefaultWidth, m_height/(m_nlayer+LAND_START+LAND_END));
	frog->save_size();
	m_frog.push_back(frog);
	if (multi == 1){
		frog = new Frog(m_width / 3, DefaultSpeed, "../res/frog1.png", this);
		frog->set_size(F_DefaultWidth, m_height/(m_nlayer+LAND_START+LAND_END));
		frog->save_size();
		m_frog.push_back(frog);
	}
	Layer *layer = new Layer(0, m_width/2, m_width, this);
	Wood * wood = new Wood (layer, m_width/2, m_width, 0, "../res/grass.jpg", this);
	wood->set_size(m_width, m_height/(m_nlayer+LAND_START+LAND_END));
	wood->save_size();
	layer->add_wood(wood);
	m_layers.push_back(layer);

	for (int i = 0; i < m_nlayer; i++){
		layer = new Layer(i+1, m_width/2, m_width, this);

		double width_factor = 1.0/(i/2+2);
		double speed_factor = i+1;

		double min_width = std::max(WoodDefaultWidth * width_factor / 4, 20.0);
		double max_width = std::max(WoodDefaultWidth * width_factor, min_width);
		double max_speed = std::min(WoodDefaultSpeed * speed_factor, 500.0);
		double min_speed = std::min(WoodDefaultSpeed * speed_factor / 2, max_speed);
		double min_space = std::max(min_width / 2, 40.0);
		double max_space = std::max(max_width * 1.5, min_space);
		max_space = std::min(layer->m_layer_width, max_space);
		min_space = std::min(min_space, max_space);

		layer->init(min_width, max_width, min_speed, max_speed, min_space, max_space);
		m_layers.push_back(layer);
	}

	layer = new Layer(m_nlayer+1, m_width/2, m_width, this);
	wood = new Wood(layer, m_width/2, m_width, 0, "../res/grass.jpg", this);
	wood->set_size(m_width, m_height/(double)(m_nlayer+LAND_START+LAND_END));
	wood->save_size();
	layer->add_wood(wood);
	m_layers.push_back(layer);

	for (int i = 0; i < m_frog.size(); i++){
		m_frog[i]->reset();
	}
}


bool Game::update(){ // update for one time.
	timer.set_stop();
	double time_remain = timer.get_time() / 1000.0;
	if (time_remain == 0.0) {
		timer.set_start();
		return true;}

	for (int i = 0; i < m_frog.size(); i++)
	{
		Frog *frog = m_frog[i];
		if (frog->m_layer == m_nlayer + 1) // game win
		{
		//	printf()
			frog->reset();
		}
	}

	for (int i = 0; i < m_layers.size(); i++){
		Layer *layer = m_layers[i];
		m_targs[i].layer = layer;
		m_targs[i].time_remain = time_remain;

		pthread_create(&m_threads[i], NULL, call_threads, &m_targs[i]);

	}

	for (int i = 0; i < m_threads.size(); i++){
		pthread_join(m_threads[i], NULL);
	}

	for (int i = 0; i < m_frog.size(); i++){
		m_frog[i]->update(time_remain);
	}

	update_canvas();
	timer.set_start();
	return true;
}


void Game::update_canvas(){
	double width_ratio = width() / m_width;
	double height_ratio = height() / m_height;

	m_canvas.set_scale(width_ratio, height_ratio);

	m_canvas.move_img(m_bg, get_pos(m_nlayer+1, m_width/2, m_bg->swidth(), m_bg->sheight()));

	for (int i = 0; i < m_layers.size(); i++){
		Layer *layer = m_layers[i];
		for (int j = 0; j < layer->m_woods.size(); j++){
			Wood *wood = layer->m_woods[j];
			m_canvas.move_img(wood, get_pos(layer->m_layer_id, wood->m_pos, wood->swidth(), wood->sheight()));
		}
	}

	for (int i = 0; i < m_frog.size(); i++){
		Frog *frog = m_frog[i];

		m_canvas.move_img(frog, get_pos(frog->m_layer, frog->m_pos, frog->swidth(), frog->sheight()));
	}

}


void Game::start(){
	set_title("Demo");
	set_default_size(m_width, m_height);
	set_resizable(true);

	init();

	//set Canvas
	m_canvas.set_img(m_bg, get_pos(m_nlayer+1, m_width/2, m_bg->swidth(), m_bg->sheight()));
	//set wood
	for(int i = 0; i < m_layers.size(); i++){
		Layer *layer = m_layers[i];
		for (int j = 0; j < layer->m_woods.size(); j++){
			Wood *wood = layer->m_woods[j];
			m_canvas.set_img(wood, get_pos(layer->m_layer_id, wood->m_pos, wood->swidth(), wood->sheight()));
		}
	}
	//set frog
	for (int i = 0; i < m_frog.size(); i++){
		Frog *frog = m_frog[i];
		m_canvas.set_img(frog, get_pos(frog->m_layer, frog->m_pos, frog->swidth(), frog->sheight()));
	}
	//show

	m_threads.resize(m_layers.size());
	m_targs.resize(m_layers.size());

	add(m_canvas);
	show_all();

	slot_up = sigc::mem_fun(*this, &Game::update);
	refresh_interval = 1000.0 / fps;
	conn = Glib::signal_timeout().connect(slot_up, refresh_interval);

	timer.set_start();
}

void Game::insert_wood(int layer, int pos){
	if (layer < m_nlayer){
		m_layers[layer+1]->insert_wood(pos);
	}
}


	bool Frog::init(){
		Layer *layer = m_game->m_layers[m_layer];
		for (int i = 0; i < layer->m_woods.size(); i++){
			Wood *wood = layer->m_woods[i];
			if (on_wood(wood)){
				m_wood = wood;
				wood->add_frog(this);
				return true;
			}
		}
		return false;
	}

	bool Frog::frog_up(){
		m_layer++;
		if (m_layer == m_game->m_nlayer+1) printf("Win!\n");
		m_wood->remove_frog(this);
		Layer *layer = m_game->m_layers[m_layer];
		m_wood = layer->find_frog(this);
		if (m_wood == 0){
			reset();
			return false;
		}
		m_wood->add_frog(this);
		return true;
	}
	bool Frog::frog_down(){
		m_layer--;
		if (m_layer < 1) return true;
		m_wood->remove_frog(this);
		Layer *layer = m_game->m_layers[m_layer];
		m_wood = layer->find_frog(this);
		if (m_wood == 0){
			reset();
			return false;
		}
		m_wood->add_frog(this);
		return true;
	}


	bool Wood::update(double time_remain){
		m_pos += m_speed * time_remain;
		for (int i = 0; i < m_frog.size(); i++)
			m_frog[i] -> set_pos(m_frog[i]->m_pos + m_speed*time_remain);
	}

