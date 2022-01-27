#include "loaderwindow.hpp"
#include "rwfont.hpp"
#include "rwimage.hpp"

LoaderWindow::LoaderWindow()
{
	is_initialized = init();
	if (is_initialized)
	{
		is_initialized = initGL();
	}
}

bool LoaderWindow::init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) 
	{
		SDL_Log("ERROR: Unable to initialize SDL: %s\n", SDL_GetError());
		return false;
	} 
	if (IMG_Init(IMG_INIT_PNG) < 0) 
	{
		SDL_Log("ERROR: Unable to initialize IMG: %s\n", IMG_GetError());
		return false;
	}

	if (TTF_Init() < 0) 
	{
		SDL_Log("ERROR: Unable to initialize TTF: %s\n", TTF_GetError());
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

	main_window = SDL_CreateWindow("Loader", 100, 100, DEFAULT_WINDOW_WIGHT, DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL);
	if (!main_window) 
	{
		SDL_Log("ERROR: Unable to create window: %s\n", SDL_GetError());
		return false;
	}

	main_gl_context = SDL_GL_CreateContext(main_window);
    	if (main_gl_context == NULL)
	{
        	SDL_Log("ERROR: Unable to creare OpenGL context: %s\n", SDL_GetError());
        	return false;
    	}

    	ptr_str_rwops = SDL_RWFromMem(rw_font_mem, rw_font_mem_len);
	rwfont = TTF_OpenFontRW(ptr_str_rwops, 1, 16);

	if (!rwfont) 
	{
		SDL_Log("ERROR: Unable to load font: %s\n", TTF_GetError());
		return false;
	}

	//ptr_img_rwops = SDL_RWFromMem(ve_rw_image, ve_rw_image_len);
	//rw_image = IMG_Load_RW(ptr_img_rwops, 1);

	//if (!rw_image)
	//{
	//	  SDL_Log("ERROR: Unable to load font: %s\n", IMG_GetError());
	//	  return false;
	//}

    	if (SDL_GL_SetSwapInterval(-1) < 0)
    	{
        	SDL_Log("WARNING: Unable to set VSync: %s\n", SDL_GetError());
    	}
    	return true;
}

bool LoaderWindow::initGL()
{
	GLenum error = GL_NO_ERROR;

	int window_height; 
	int window_width; 

	SDL_GetWindowSize(main_window, &window_width, &window_height);

	glViewport(0.0f, 0.0f, window_width, window_height);

	glMatrixMode(GL_PROJECTION);
    	glLoadIdentity();

    	glOrtho(0.0f, window_width, 0.0f, window_height, 0.0f, 1.0f);

    	error = glGetError();
    	if (error != GL_NO_ERROR)
    	{
        	//SDL_Log("ERROR: Unable to initialize OpenGL: %s\n", gluErrorString(error));
        	return false;
    	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		//SDL_Log("ERROR: Unable to initialize OpenGL: %s\n", gluErrorString(error));
		return false;
	}

	glEnable(GL_ALPHA_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);

	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		//SDL_Log("ERROR: Unable to initialize OpenGL: %s\n", gluErrorString(error));
		return false;
	}
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	return true;
}

double LoaderWindow::fRand(double fMin, double fMax)
{
	double f = (double)std::rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}

void LoaderWindow::createDots()
{
	int window_height; 
	int window_width; 

	SDL_GetWindowSize(main_window, &window_width, &window_height);

	for (size_t index = 0; index < MAX_DOT_AMOUNT; ++index)
	{
		double speed_x = fRand(MIN_DOT_SPEED, MAX_DOT_SPEED * 2.0f);
		double speed_y = fRand(MIN_DOT_SPEED, MAX_DOT_SPEED * 2.0f);
		dots.push_back((dot){speed_x - MAX_DOT_SPEED, speed_y - MAX_DOT_SPEED, static_cast<double>(std::rand() % window_width), static_cast<double>(std::rand() % window_height), MAX_DOT_RADIUS});
	}
}

double LoaderWindow::calculateDistance(dot point_1, dot point_2)
{
	return std::sqrt(std::pow(point_2.pos_x - point_1.pos_x, 2) + std::pow(point_2.pos_y - point_1.pos_y, 2));
}

void LoaderWindow::drawString(std::string message, SDL_Color color, int x, int y, TTF_Font* font) 
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	SDL_Surface* sFont = TTF_RenderText_Blended(font, message.c_str(), color);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sFont->w, sFont->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, sFont->pixels);

	glBegin(GL_QUADS);
	{
		glTexCoord2f(0,1); glVertex2f(x, y);
		glTexCoord2f(1,1); glVertex2f(x + sFont->w, y);
		glTexCoord2f(1,0); glVertex2f(x + sFont->w, y + sFont->h);
		glTexCoord2f(0,0); glVertex2f(x, y + sFont->h);
	}
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glDeleteTextures(1, &texture);
	SDL_FreeSurface(sFont);
}

void LoaderWindow::drawIMG(int x, int y, SDL_Surface *image) 
{
  	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  	GLuint texture;
  	glGenTextures(1, &texture);
  	glBindTexture(GL_TEXTURE_2D, texture);

  	

  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

  	glBegin(GL_QUADS);
  	{
    	glTexCoord2f(0,1); glVertex2f(x, y);
    	glTexCoord2f(1,1); glVertex2f(x + image->w, y);
    	glTexCoord2f(1,0); glVertex2f(x + image->w, y + image->h);
    	glTexCoord2f(0,0); glVertex2f(x, y + image->h);
  	}
  	glEnd();

  	glMatrixMode(GL_PROJECTION);
  	glPopMatrix();

  	glDeleteTextures(1, &texture);
}

std::vector<dot> LoaderWindow::findNearestDot(std::vector<dot> points, size_t point_index, size_t k)
{
	std::vector<dot> temp;
	std::vector<double> buffer;
    
	for (size_t index = 0; index < MAX_DOT_AMOUNT; ++index)
	{
		buffer.push_back(calculateDistance(points[point_index], points[index]));
	}

	for (size_t index = 0; index < k; ++index)
	{
		if (index != point_index)
		{
			auto buffer_min_element = std::min_element(buffer.begin(), buffer.end());
			temp.push_back(points[std::distance(buffer.begin(), buffer_min_element)]);
			if (*buffer_min_element < MAX_DOT_FADE)
			{
				temp.back().transp = (1 - *buffer_min_element / (1 / 1) / MAX_DOT_FADE) * 255;
			}
			else 
			{
				temp.back().transp = 0;
			}
			buffer[std::distance(buffer.begin(), std::min_element(buffer.begin(), buffer.end()))] = 9999999;
		}
		else 
		{
			k++;
		}
	}
	return temp;
}

void LoaderWindow::drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius)
{
	int i;
	int triangleAmount = 6; 

	GLfloat twicePi = 2.0f * PI;
	
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y); // center of circle
		for(i = 0; i <= triangleAmount;i++) 
		{ 
			glVertex2f(x + (radius * cos(i *  twicePi / triangleAmount)), y + (radius * sin(i * twicePi / triangleAmount)));
		}
	glEnd();
}

void LoaderWindow::render()
{
	int window_height; 
	int window_width;

	SDL_GetWindowSize(main_window, &window_width, &window_height);

	deltaclock = SDL_GetTicks() - startclock;
	startclock = SDL_GetTicks();

	if (deltaclock != 0)  
	{
		currentFPS = 1000 / deltaclock; 
	}
	std::string s = std::to_string(currentFPS);
	s = "FPS: " + s;

	glClear(GL_COLOR_BUFFER_BIT);

	for (size_t index = 0; index < MAX_DOT_AMOUNT; ++index)
	{
		std::vector<dot> temp_dots;
 		
 		glColor4ub(RED_COLOUR, 255);

		drawFilledCircle(dots[index].pos_x, dots[index].pos_y, MAX_DOT_RADIUS);

		temp_dots = findNearestDot(dots, index, MAX_DOT_CONNECTION);

		//glLineWidth(2);
		for (size_t index_i = 0; index_i < MAX_DOT_CONNECTION; ++index_i)
		{
			if (temp_dots[index_i].transp > 1)
			{
				glColor4ub(RED_COLOUR, temp_dots[index_i].transp);
				glBegin(GL_LINES);
				    glVertex2f(dots[index].pos_x, dots[index].pos_y);
				    glVertex2f(temp_dots[index_i].pos_x, temp_dots[index_i].pos_y);
				glEnd();
			}
		}
	}

	glColor4ub(0, 0, 0, 200);

	glBegin(GL_QUADS);
  	{
    	glVertex2f(0, window_height);
    	glVertex2f(window_width, window_height);
    	glVertex2f(window_width, window_height - MAX_TITLEBAR_HEIGHT);
    	glVertex2f(0, window_height - MAX_TITLEBAR_HEIGHT);
  	}
  	glEnd();

	glColor4ub(255, 255, 255, 255);

	drawString(s.c_str(), SDL_Color{255, 255, 255}, 10, 10, rwfont);

	//drawIMG(10, 10, rw_image);

	if (last_height != window_height || last_width != window_width)
	{
		last_height = window_height;
		last_width = window_width;

		glViewport(0.0f, 0.0f, window_width, window_height);
	    glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();

	    glOrtho(0.0f, window_width, 0.0f, window_height, 0.0f, 1.0f);
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();
	}

	SDL_GL_SwapWindow(main_window);
	glFlush();
}

void LoaderWindow::update()
{
	int window_height; 
	int window_width;

	SDL_GetWindowSize(main_window, &window_width, &window_height);

	for (size_t index = 0; index < MAX_DOT_AMOUNT; ++index)
	{
		if ((dots[index].pos_x + dots[index].speed_x < 0 + MAX_DOT_RADIUS) || (dots[index].pos_x + dots[index].speed_x > window_width - MAX_DOT_RADIUS))
		{

			if (dots[index].pos_x + dots[index].speed_x * 2 < 0 + MAX_DOT_RADIUS)
			{
				dots[index].pos_x = 0 + MAX_DOT_RADIUS;
			}
			if (dots[index].pos_x + dots[index].speed_x * 2 > window_width - MAX_DOT_RADIUS)
			{
				dots[index].pos_x = window_width - MAX_DOT_RADIUS;
			}

			dots[index].speed_x *= -1;
		}

		if ((dots[index].pos_y + dots[index].speed_y < 0 + MAX_DOT_RADIUS) || (dots[index].pos_y + dots[index].speed_y > window_height - MAX_DOT_RADIUS))
		{

			if (dots[index].pos_y + dots[index].speed_y * 2 < 0 + MAX_DOT_RADIUS)
			{
				dots[index].pos_y = 0 + MAX_DOT_RADIUS;
			}
			if (dots[index].pos_y + dots[index].speed_y * 2 > window_height - MAX_DOT_RADIUS)
			{
				dots[index].pos_y = window_height - MAX_DOT_RADIUS;
			}

			dots[index].speed_y *= -1;
		}

		dots[index].pos_x += dots[index].speed_x;
		dots[index].pos_y += dots[index].speed_y;
	}
}

int LoaderWindow::renderThread(void* pointer)
{
	SDL_GL_MakeCurrent(((LoaderWindow*)pointer)->main_window, ((LoaderWindow*)pointer)->main_gl_context);
	while(((LoaderWindow*)pointer)->is_active)
	{
		((LoaderWindow*)pointer)->render();
		SDL_Delay(UPADTE_DELAY);
	}
	return 1;
}

int LoaderWindow::updateThread(void* pointer)
{
	while(((LoaderWindow*)pointer)->is_active)
	{
		((LoaderWindow*)pointer)->update();
		SDL_Delay(UPADTE_DELAY);
	}
	return 1;
}

void LoaderWindow::keyPressHandler(unsigned char event, int x, int y)
{
	switch(event)
	{
		case SDLK_ESCAPE:
		{
			is_active = false;
		}
		break;
	}
}

void LoaderWindow::windowEventHandler(unsigned char event)
{
	switch(event)
	{
		case SDL_WINDOWEVENT_EXPOSED:
		{
			
		}
		break;
	}
}

void LoaderWindow::buttonPressHandler(unsigned char event, int x, int y)
{
	switch(event)
	{
		case SDL_BUTTON_LEFT:
		{
			if (y < MAX_TITLEBAR_HEIGHT)
			{
				int x_g = 0, y_g = 0;
				SDL_GetGlobalMouseState(&x_g, &y_g);
				last_mouse_x = x_g;
				last_mouse_y = y_g;
				is_trakcing = true;
				SDL_CaptureMouse(SDL_TRUE);
			}
		}
		break;
	}
}

void LoaderWindow::start()
{
	if (!is_initialized)
	{
		SDL_Log("ERROR: LoaderWindow is not properly initialized, process will be terminated");
		return;
	}      

	createDots();

	startclock = SDL_GetTicks();
	deltaclock = SDL_GetTicks();

	is_active = true;

	SDL_GL_MakeCurrent(NULL, NULL);

	render_thread = SDL_CreateThread(LoaderWindow::renderThread, "renderThread", this);
	update_thread = SDL_CreateThread(LoaderWindow::updateThread, "updateThread", this);

	while(is_active)
	{
		while(SDL_PollEvent(&main_event) != 0)
		{
			switch (main_event.type)
			{
				case SDL_QUIT:
				{
					is_active = false;
				}
				break;

				case SDL_KEYDOWN:
				{
					int x = 0, y = 0;
					SDL_GetMouseState(&x, &y);
					keyPressHandler(main_event.key.keysym.sym, x, y);
				}
				break;

				case SDL_WINDOWEVENT:
				{
					windowEventHandler(main_event.window.event);
				}
				break;

				case SDL_MOUSEBUTTONDOWN:
				{
					int x = 0, y = 0;
					SDL_GetMouseState(&x, &y);
					buttonPressHandler(main_event.button.button, x, y);
				}
				break;

				case SDL_MOUSEBUTTONUP:
				{
					is_trakcing = false;
					SDL_CaptureMouse(SDL_FALSE);
				}
				break;

				case SDL_MOUSEMOTION :
				{
					if (is_trakcing)
					{
						int x = 0, y = 0;
						SDL_GetGlobalMouseState(&x, &y);
						if (x != last_mouse_x || y != last_mouse_y)
						{
							int win_x = 0, win_y = 0;
							SDL_GetWindowPosition(main_window, &win_x, &win_y);
							SDL_SetWindowPosition(main_window, win_x + (x - last_mouse_x), win_y + (y - last_mouse_y));
							last_mouse_x = x;
							last_mouse_y = y;
						}
					}
				}
			}
		}
		SDL_Delay(UPADTE_DELAY);
	}
}

void LoaderWindow::close()
{
	SDL_WaitThread(render_thread, NULL);
	SDL_WaitThread(update_thread, NULL);
	SDL_DestroyWindow(main_window);
	main_window = NULL;

	SDL_Quit();
	TTF_Quit();
	IMG_Quit();
}

LoaderWindow::~LoaderWindow()
{
	close();
}
