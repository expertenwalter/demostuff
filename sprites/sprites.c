#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include <SDL/SDL.h>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  Uint32 rmask = 0xff000000;
  Uint32 gmask = 0x00ff0000;
  Uint32 bmask = 0x0000ff00;
  Uint32 amask = 0x000000ff;
#else
  Uint32 rmask = 0x000000ff;
  Uint32 gmask = 0x0000ff00;
  Uint32 bmask = 0x00ff0000;
  Uint32 amask = 0xff000000;
#endif

// Graphics constants
static const int WIDTH = 640;
static const int HEIGHT = 480;
static const int DEPTH = 32;

// Mathematical constants
static const float MATH_PI = 3.1415926536;

// Screen
SDL_Surface* screen;
SDL_Surface* ball_surface;

// Precalculated values
float sintable[360];


/**
 * Put a pixel onto an SDL surface (32 bit depth only)
 */
void sdl_putpixel(SDL_Surface *surface, int x, int y, char r, char g, char b, char a)
{
  int bpp = surface->format->BytesPerPixel;

  /* Here p is the address to the pixel we want to set */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  Uint32 pixel = ((r << 24) & rmask) + ((g << 16) & gmask) + ((b << 8) & bmask) + (a & amask);
#else
  Uint32 pixel = ((a << 24)& amask) + ((b << 16) & bmask) + ((g << 8) & gmask) + (r & rmask);
#endif

  *(Uint32 *)p = pixel;
}


/**
 * Init the sine table
 */
void init_sin_table()
{
  for(int a = 0; a < 360; a++)
  {
    sintable[a] = sin((a * MATH_PI) / 180.0);
  }
}


/**
 * Fast Sine based on precalculated values
 */
float fast_sin(float a)
{
  return sintable[(int) (a * 180.0 / MATH_PI) % 360];
}


/**
 * Fast cosine based on precalculated sine values
 */
float fast_cos(float a)
{
  return sintable[((int) (a * 180.0 / MATH_PI) + 90) % 360];
}


SDL_Surface* sprite_create_ball()
{
  SDL_Surface* temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 32, 32, 32,
                                   rmask, gmask, bmask, amask);
  
  for(int y = 0; y < 32; y++)
      for(int x = 0; x < 32; x++)
	sdl_putpixel(temp, x, y, 240 * fast_sin((float) (x / 32.0) * MATH_PI) * fast_sin((float) (y / 32.0) * MATH_PI), 0, 0, 40 * fast_sin((float) (x / 32.0) * MATH_PI) * fast_sin((float) (y / 32.0) * MATH_PI));
  
  return temp;
}


/**
 * Draw ball animation
 */
void draw_balls(SDL_Surface* screen, int time)
{
    SDL_Rect spritepos;
    
    // Clear screen
    SDL_FillRect(screen, NULL, 0x00000000);
    
    // Draw 10 balls
    for(int i = 24; i < 360; i++)
    {
      spritepos.x = (WIDTH / 2) + (HEIGHT / 3) * fast_cos(((i * (360.0 / 24) + time) / 180.0) * MATH_PI);
      spritepos.y = (HEIGHT / 2) + (HEIGHT / 3) * fast_sin(((i * (360.0 / 24)  + time) / 180.0) * MATH_PI);
    
      SDL_BlitSurface(ball_surface, NULL, screen, &spritepos);
    }
}


/**
 * Init SDL and all subsystems
 */
void sdl_init()
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0 )
    exit(EXIT_FAILURE);

  if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, 0)))
  {
    SDL_Quit();
    exit(EXIT_FAILURE);
  }
}


int main(int argc, char** argv)
{
  SDL_Event event;

  int keypress = 0;
  int t = 0;
  
  // Init everything
  sdl_init();
  init_sin_table();
  ball_surface = sprite_create_ball();
  
  // Loop until the end
  while(!keypress) 
  {
    // Lock SDL surface if needed
    if(SDL_MUSTLOCK(screen)) 
      if(SDL_LockSurface(screen) < 0)
	exit(EXIT_FAILURE);
  
    // Draw the plasma
    draw_balls(screen, t);

    // Unlock the SDL surface if needed
    if(SDL_MUSTLOCK(screen))
      SDL_UnlockSurface(screen);
  
    // Flip surfaces
    SDL_UpdateRect(screen, 0, 0, 0, 0);
      
    // Handle SDL events
    while(SDL_PollEvent(&event)) 
    {      
      switch (event.type) 
      {
	case SDL_QUIT:
	case SDL_KEYDOWN:
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
  SDL_Quit();
  return EXIT_SUCCESS;
}
