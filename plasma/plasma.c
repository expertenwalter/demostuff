#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include <SDL/SDL.h>


static const int WIDTH = 640;
static const int HEIGHT = 480;
static const int DEPTH = 32;

static const float MATH_PI = 3.1415926536;

SDL_Surface *screen;

Uint32 palette[256];
float sintable[360];



void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    *(Uint32 *)p = pixel;
}


void initPalette()
{
  for(int i = 0; i < 256; i++)
    palette[i] = (i << 16) + (i / 2 << 8) + (256 - i);
}


void initSDL()
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0 )
    exit(EXIT_FAILURE);

  if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, 0)))
  {
    SDL_Quit();
    exit(EXIT_FAILURE);
  }
}


void initSinTable()
{
  for(int a = 0; a < 360; a++)
  {
    sintable[a] = sin((a * MATH_PI) / 180.0);
  }
}


float fastSin(float a)
{
  return sintable[(int) (a * 180.0 / MATH_PI) % 360];
}


float fastCos(float a)
{
  return sintable[((int) (a * 180.0 / MATH_PI) + 90) % 360];
}


void drawPlasma(SDL_Surface* screen, int time)
{
  for(int y = 0; y < HEIGHT; y++)
    for(int x = 0; x < WIDTH; x++)
    {
      float func1 = 64 + 63 * (fastSin((hypot((HEIGHT / 2) - y, (WIDTH / 2) - x) / 16) + (time / 2.0)));
      float func2 = 64 + 63 * fastSin(x / (37.0 + 15.0 * fastCos(y / 74.0))) * fastCos(y / (31.0 + 11.0 * fastSin(x / 57.0)));
      putpixel(screen, x, y, palette[(int) (func1 + func2)]);
    }
}


int main(int argc, char** argv)
{
  SDL_Event event;

  int keypress = 0;
  int t = 0;
  
  initSDL();
  initPalette();
  initSinTable();
  
  while(!keypress) 
  {
    if(SDL_MUSTLOCK(screen)) 
      if(SDL_LockSurface(screen) < 0)
	exit(EXIT_FAILURE);
  
    drawPlasma(screen, t++);
    
    if(SDL_MUSTLOCK(screen))
      SDL_UnlockSurface(screen);
  
    SDL_Flip(screen);
      
    while(SDL_PollEvent(&event)) 
    {      
      switch (event.type) 
      {
	case SDL_QUIT:
	  keypress = 1;
	  break;
      }
    }
    
    usleep(40000);
  }
  
  return EXIT_SUCCESS;
}
