#include <SDL2/SDL.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include <math.h>
#include "portaudio.h"

const int MAX_OBSTACLES = 250;
const int SCREEN_HEIGHT = 800;
const int SCREEN_WIDTH =800;
const int OBSTACLE_SIZE = 20;
const int FLOOR_HEIGHT = 100;
const int BALL_X = 200;
const int STARTING_BALL_Y = 580;
const int BALL_RADIUS=10;
const int FRAME_LEFT_INCREMENT =1;
const float GRAVITY_TIME_INTERVAL = 0.2f;
const float FRAME_LEFT_INTERVAL = 0.2f;
const double PI = 3.14159265;

using namespace std;

enum Color { RED, BLUE, GREEN, ORANGE, CYAN, YELLOW, MAGENTA, AMBER };

struct Ball
{
    const int x = BALL_X;
    int y;
    const int radius = BALL_RADIUS;
    Color color = GREEN;
};
struct Obstacle
{
    int l;
    int r;
    int t;
    int b;
    Color color = RED;
};
struct Floor
{
    const int height = SCREEN_HEIGHT-FLOOR_HEIGHT;
    Color color = AMBER;
};


class Controller
{
public:
    Controller();
    ~Controller();

    void InitializeGraphics();
    void ClearScreen();
    void UpdateScreen();
    void DrawFloor(const int height, Color color);
    void DrawCircle(const int h, const int k, const int radius, const Color color);
	void DrawRectangle(const int x, const int y, const int width, const int height, const Color color);
    bool HasUserQuit();

private:
    SDL_Window *_window;
    SDL_Renderer *_renderer;
    void SetRendererColor(const Color color);
};
void PushObstacles(vector<Obstacle> *obsvec, int l, int r, int t, int b, Color color)
{
    Obstacle *obs = new Obstacle;
    obs->b=b;
    obs->l=l;
    obs->t=t;
    obs->r=r;
    obs->color=color;
    obsvec->push_back(*obs);
}

int main(int argc, char * args[])
{
    Controller controller;
    Ball ball;
    ball.y=690;
    vector<Obstacle> obstacles;
    Floor floor;
                                                               
    int arr[42]={1,2,3,4,5,6,8,46,50,54,16,32,15,46,59,59,0,8,12,49,5,1,3,29,56,49,0,0,0,0,0,0,12,45,35,36,37,38,39,39,39,39};
    int i =1;
    while(i<42)
    {   
        if(i%3==1)
            PushObstacles(&obstacles,ball.x+i*100,(ball.x+i*100)+100,floor.height-arr[i]*5,floor.height,RED);
        else if(i%8==0)
            PushObstacles(&obstacles,ball.x+i*100,(ball.x+i*100)+100,floor.height-arr[i]*6,floor.height,BLUE);
        else if(i%2==0)
            PushObstacles(&obstacles,ball.x+i*100,(ball.x+i*100)+100,floor.height-arr[i]*5,floor.height,AMBER);
        else
            PushObstacles(&obstacles,ball.x+i*100,(ball.x+i*100)+100,floor.height-arr[i]*6,floor.height,YELLOW);
        i++;
    }

    
    unsigned long leftground=SDL_GetTicks();
    int ballcounter=0;
    int balljump=false;

    int lastbally=ball.y;

    bool ballfall=false;
    int fallcounter=0;

    bool foundvirtualfloor = false;
    int virtualfloorheight=floor.height;
    int YTakeoff;
    bool foundvirtualcieling=false;
    int virtualcielingheight=0;


    int ballbot=ball.y+ball.radius;
    int balltop=ball.y-ball.radius;
    int ballleft=ball.x-ball.radius;
    int ballright=ball.x+ball.radius;

    int keyPressed=-1;
	int keyReleased=-1;
	int originalIteration=1; //set to 0 at end of 1st press loop
	unsigned long timePressed = SDL_GetTicks(); //



    while (!controller.HasUserQuit())
    {
        controller.ClearScreen();
        ballbot=ball.y+ball.radius;
        balltop=ball.y-ball.radius;
        ballleft=ball.x-ball.radius;
        ballright=ball.x+ball.radius;
        virtualfloorheight=floor.height;
        //-----------------------------------------------------------------------------------
        for(int i =0;i<obstacles.size();i++)
        {
            controller.DrawRectangle(
                obstacles[i].l,
                obstacles[i].t,
                (obstacles[i].r-obstacles[i].l),
                (obstacles[i].b-obstacles[i].t),
                obstacles[i].color);
            obstacles[i].l--;
            obstacles[i].r--;
            if ((obstacles[i].l<=ballright&&obstacles[i].l>=ballleft)||(obstacles[i].r>=ballleft&&obstacles[i].r<=ballright)||(ballleft>obstacles[i].l&&ballright<obstacles[i].r))
            {
                virtualfloorheight=obstacles[i].t;

            }
        }

        if (balljump)
        {
            ballcounter++;
            if(ballcounter>=190)
            {
                ballcounter=0;
                balljump=false;
            }
            if(ballcounter==1)
                YTakeoff=virtualfloorheight;
            if(ballcounter!=0)
            {

                ballcounter++;
                ball.y =YTakeoff - (int) 330 * sin ((double)ballcounter*PI/189);
                ball.y-=ball.radius;
            }
        }
        if(ball.y+ball.radius>virtualfloorheight)
        {
            fallcounter=0;
            balljump=false;
            ballcounter=0;
            ball.y=virtualfloorheight-ball.radius;
        }

        if(!balljump&&ball.y+ball.radius<virtualfloorheight)
        {
            fallcounter++;
            ball.y+=2*fallcounter;
        }

        usleep(7500);
        
        //----------------------------------------------------------------------------------
        controller.DrawFloor(floor.height,floor.color);
        controller.DrawCircle(ball.x,ball.y,ball.radius,ball.color);
        controller.UpdateScreen();

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
			if(e.type==SDL_KEYDOWN && e.key.keysym.sym!=keyPressed)
			{
				keyPressed=e.key.keysym.sym;
				timePressed = SDL_GetTicks();
				originalIteration=1;
				break;
			}
			if(e.type==SDL_KEYUP)
			{
				keyPressed=-1;
				keyReleased=e.key.keysym.sym;
				break;
			}
			else if (e.type==SDL_QUIT)
				exit(1);
        }

        switch(keyPressed)
        {
            case SDLK_UP:
                //if ball on ground, then jump
                if(originalIteration==1)
                {
                    originalIteration=0;
                    
                    balljump=true;
                    
                    timePressed=SDL_GetTicks();

                }
                if(SDL_GetTicks()-timePressed > 100)
                {
                    balljump=true;
                    timePressed=SDL_GetTicks();
                }
                break;
        }


    }



    return 0;
}
//--------------------------------------------------------------------------------
Controller::Controller()
{
	_renderer = nullptr;
	_window = nullptr;
	InitializeGraphics();
}

Controller::~Controller()
{
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	_renderer = nullptr;
	_window = nullptr;
	SDL_Quit();
}

void Controller::InitializeGraphics()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		cerr << "Init Error " << SDL_GetError() << endl;
		return;
	}
	_window = SDL_CreateWindow("Jumpgame", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 800, SDL_WINDOW_SHOWN);
	if (_window == nullptr)
	{
		cerr << "Window Error " << SDL_GetError() << endl;
		return;
	}
	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
	if (_renderer == nullptr)
	{
		cerr << "Renderer Error" << SDL_GetError() << endl;
		return;
	}

}

void Controller::SetRendererColor(const Color color)
{
    switch (color)
    {
        case RED:
            SDL_SetRenderDrawColor(_renderer, 0xFF, 0x00, 0x00, 0xFF);
            break;
        case BLUE:
            SDL_SetRenderDrawColor(_renderer, 0x00, 0x00, 0xFF, 0xFF);
            break;
        case AMBER:
            SDL_SetRenderDrawColor(_renderer, 0xFF, 0xBF, 0xF0, 0xFF);
            break;
        case GREEN:
            SDL_SetRenderDrawColor(_renderer, 0x00, 0xFF, 0x00, 0xFF);
            break;
        case CYAN:
            SDL_SetRenderDrawColor(_renderer, 0x00, 0xFF, 0xFF, 0xFF);
            break;
        case MAGENTA:
            SDL_SetRenderDrawColor(_renderer, 0xFF, 0x00, 0xFF, 0xFF);
            break;
        case ORANGE:
            SDL_SetRenderDrawColor(_renderer, 0xFF, 0xA5, 0x00, 0xFF);
            break;
        case YELLOW:
            SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0x00, 0xFF);
            break;
    }
}
void Controller::ClearScreen()
{
	SDL_SetRenderDrawColor(_renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(_renderer);
}

void Controller::UpdateScreen()
{
	SDL_RenderPresent(_renderer);
}
void Controller::DrawCircle(const int h, const int k, const int radius, const Color color)
{
   const int diameter = (radius * 2);

   int x = (radius - 1);
   int y = 0;
   int tx = 1;
   int ty = 1;
   int error = (tx - diameter);
   SetRendererColor(color);
   while (x >= y)
   {
      SDL_RenderDrawPoint(_renderer, h + x, k - y);
      SDL_RenderDrawPoint(_renderer, h + x, k + y);
      SDL_RenderDrawPoint(_renderer, h - x, k - y);
      SDL_RenderDrawPoint(_renderer, h - x, k + y);
      SDL_RenderDrawPoint(_renderer, h + y, k - x);
      SDL_RenderDrawPoint(_renderer, h + y, k + x);
      SDL_RenderDrawPoint(_renderer, h - y, k - x);
      SDL_RenderDrawPoint(_renderer, h - y, k + x);

      if (error <= 0)
      {
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         --x;
         tx += 2;
         error += (tx - diameter);
      }
   }
}
void Controller::DrawFloor(const int height, Color color)
{
    SetRendererColor(color);
    SDL_RenderDrawLine(_renderer,0,height,SCREEN_WIDTH,height);
}
void Controller::DrawRectangle(const int x, const int y, const int width, const int height, const Color color)
{
	SDL_Rect fillArea = { x, y, width, height };
    SetRendererColor(color);
	SDL_RenderFillRect(_renderer, &fillArea);
}

bool Controller::HasUserQuit()
{
	return false;
}
