#ifndef LOADERWINDOW_HPP
#define LOADERWINDOW_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>


#define DEFAULT_WINDOW_HEIGHT	480
#define DEFAULT_WINDOW_WIGHT	640
#define RED_COLOUR 				150, 0, 24
#define BLACK_COLOUR 			0, 0, 0, 255
#define MAX_DOT_AMOUNT 			80
#define MAX_DOT_SPEED 			0.1f
#define MIN_DOT_SPEED 			0.01f
#define MAX_DOT_RADIUS 			2.0f
#define MAX_DOT_CONNECTION 		15
#define ANIMATED_BACKGROUND 	1
#define MAX_DOT_FADE 			140
#define MAX_WINODOW_RADIUS 		20
#define MAX_THICKNESS 			25
#define UPADTE_DELAY 			1
#define PI 						3.14f
#define MAX_TITLEBAR_HEIGHT 	DEFAULT_WINDOW_WIGHT / 20

extern "C"

struct dot 
{
	double speed_x;
	double speed_y;
	double pos_x;
	double pos_y;
	double radius;
	double transp;
};

class LoaderWindow
{
public:
	LoaderWindow();
	~LoaderWindow();
	
	void start();
	void update();
	void render();
	void close();
	void createDots();

	void drawString(std::string, SDL_Color, int, int, TTF_Font*);
	void drawFilledCircle(GLfloat, GLfloat, GLfloat);
	void drawIMG(int x, int y, SDL_Surface*);

	void keyPressHandler(unsigned char, int, int);
	void buttonPressHandler(unsigned char, int, int);
	void windowEventHandler(unsigned char);

	bool init();	
	bool initGL();

	double fRand(double, double);
	double calculateDistance(dot, dot);

	static int renderThread(void*);
	static int updateThread(void*);

	std::vector<dot> findNearestDot(std::vector<dot>, size_t, size_t);

private:
	SDL_Event 		main_event;

	SDL_Thread		*render_thread;
	SDL_Thread		*update_thread;
	SDL_Window		*main_window;
	SDL_GLContext	main_gl_context;
	TTF_Font		*rwfont;
	SDL_RWops    	*ptr_str_rwops;
	//SDL_RWops    	*ptr_img_rwops;
	//SDL_Surface 	*rw_image;

	bool 			is_initialized 	= false;
	bool 			is_active 		= false;
	bool 			is_trakcing 	= false;

	int  			last_height 	= 0;
	int 			last_width 		= 0;	

	int 			last_mouse_x;
	int 			last_mouse_y;

	Uint32 			startclock 		= 0;
	Uint32 			deltaclock 		= 0;
	Uint32 			currentFPS 		= 0;

	std::vector<dot>	dots; 
};

#endif