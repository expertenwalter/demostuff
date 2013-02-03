#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include <SDL/SDL.h>

// Graphics constants
static const int WIDTH = 640;
static const int HEIGHT = 480;
static const int DEPTH = 32;

// Mathematical constants
static const float MATH_PI = 3.1415926536;

// Screen
SDL_Surface *screen;

// Precalculated values
Uint32 palette[256];
float sintable[360];


/**
 * Put a pixel onto an SDL surface (32 Bit depth only)
 */
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    *(Uint32 *)p = pixel;
}


/**
 * Init the color palette for the plasma
 */
void initPalette()
{
  for(int i = 0; i < 256; i++)
    palette[i] = (i << 16) + (i / 2 << 8) + (256 - i);
}


/**
 * Init the sine table
 */
void initSinTable()
{
  for(int a = 0; a < 360; a++)
  {
    sintable[a] = sin((a * MATH_PI) / 180.0);
  }
}


/**
 * Fast Sine based on precalculated values
 */
float fastSin(float a)
{
  return sintable[(int) (a * 180.0 / MATH_PI) % 360];
}


/**
 * Fast cosine based on precalculated sine values
 */
float fastCos(float a)
{
  return sintable[((int) (a * 180.0 / MATH_PI) + 90) % 360];
}


/**
 * Init SDL and all subsystems
 */
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


/**
 * Draw the plasma animation at the given timestep onto the given surface
 */
void drawPlasma(SDL_Surface* screen, int time)
{
  for(int y = 0; y < HEIGHT; y++)
    for(int x = 0; x < WIDTH; x++)
    {
      // Func1: Moving circles
      float func1 = 64 + 63 * (fastSin((hypot((HEIGHT / 2) - y, (WIDTH / 2) - x) / 16) + (time / 2.0)));
      
      // Func2: The static plasma
      float func2 = 64 + 63 * fastSin(x / (37.0 + 15.0 * fastCos(y / 74.0))) * fastCos(y / (31.0 + 11.0 * fastSin(x / 57.0)));
      
      // Combine the two functions and dra the pixel
      putpixel(screen, x, y, palette[(int) (func1 + func2)]);
    }
}


int main(int argc, char** argv)
{
  SDL_Event event;

  int keypress = 0;
  int t = 0;
  
  // Init everything
  initSDL();
  initPalette();
  initSinTable();
  
  // Loop until the end
  while(!keypress) 
  {
    // Lock SDL surface if needed
    if(SDL_MUSTLOCK(screen)) 
      if(SDL_LockSurface(screen) < 0)
	exit(EXIT_FAILURE);
  
    // Draw the plasma
    drawPlasma(screen, t);
    
    // Unlock the SDL surface if needed
    if(SDL_MUSTLOCK(screen))
      SDL_UnlockSurface(screen);
  
    // Flip surfaces
    SDL_Flip(screen);
      
    // Handle SDL events
    while(SDL_PollEvent(&event)) 
    {      
      switch (event.type) 
      {
	case SDL_QUIT:
	  keypress = 1;
	  break;
      }
    }
    
    // Sleep until next frame (25 FPS)
    usleep(40000);
    
    // Advance time
    t++;
  }
  
  // Exit gracefully
  return EXIT_SUCCESS;
}
