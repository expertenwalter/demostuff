#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

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

// Effect constants
static const int TEXTUREID_CHECKER_RED = 0;
static const int TEXTUREID_CHECKER_BLUE = 1;
static const int TEXTUREID_CHECKER_GREEN = 2;
static const int TEXTUREID_CHECKER_YELLOW = 3;
static const int TEXTUREID_CHECKER_PURPLE = 4;
static const int TEXTUREID_CHECKER_CYAN = 5;
#define TEXTUREID_MAX  6

GLuint textures[TEXTUREID_MAX] = {0};

// Screen
SDL_Surface* screen;

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


/**
 * Create a colored checkerboard texture
 */
void texture_create_checkerboard(int textureid, float rparam, float bparam, float gparam)
{
  SDL_Surface* temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 128, 128, 32,
                                   rmask, gmask, bmask, amask);
  
  for(int y = 0; y < 128; y++)
    for(int x = 0; x < 128; x++)
      sdl_putpixel(temp, x, y,
	64 + rparam * 127 * ((x % 16 < 8) * (y % 16 < 8)),
	64 + bparam * 127 * ((x % 16 < 8) * (y % 16 < 8)),
	64 + gparam * 127 * ((x % 16 < 8) * (y % 16 < 8)),
	255
      );

  // Bind surface to texture
  glBindTexture(GL_TEXTURE_2D, textures[textureid]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, temp->w, temp->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp->pixels);
 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


/**
 * Initialize the textures
 */
void init_textures()
{
  // Generate textures
  glGenTextures(TEXTUREID_MAX, textures);
  
  // Checkerboard textures
  texture_create_checkerboard(TEXTUREID_CHECKER_RED, 1.0f, 0.0f, 0.0f);
  texture_create_checkerboard(TEXTUREID_CHECKER_BLUE, 0.0f, 1.0f, 0.0f);
  texture_create_checkerboard(TEXTUREID_CHECKER_GREEN, 0.0f, 0.0f, 1.0f);
  texture_create_checkerboard(TEXTUREID_CHECKER_YELLOW, 1.0f, 1.0f, 0.0f);
  texture_create_checkerboard(TEXTUREID_CHECKER_PURPLE, 0.6f, 0.2f, 0.9f);
  texture_create_checkerboard(TEXTUREID_CHECKER_CYAN, 0.0f, 0.1f, 0.1f);
}


/**
 * Draw cube
 */
void draw_cube(SDL_Surface* screen, int time)
{
  //Clear screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // Rotate
  glPushMatrix();
  glRotatef(time, 0.1f, 0.1f, 0.0f);
  
  
  // Draw the cube
  // Front
  glBindTexture(GL_TEXTURE_2D, textures[TEXTUREID_CHECKER_RED]);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( -0.5f, -0.5f, -0.5f );
    glTexCoord2f(1, 0); glVertex3f(  0.5f, -0.5f, -0.5f );
    glTexCoord2f(1, 1); glVertex3f(  0.5f,  0.5f, -0.5f );
    glTexCoord2f(0, 1); glVertex3f( -0.5f,  0.5f, -0.5f );
  glEnd();
  
  // Left
  glBindTexture(GL_TEXTURE_2D, textures[TEXTUREID_CHECKER_BLUE]);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( -0.5f, -0.5f, -0.5f );
    glTexCoord2f(1, 0); glVertex3f( -0.5f, -0.5f,  0.5f );
    glTexCoord2f(1, 1); glVertex3f( -0.5f,  0.5f,  0.5f );
    glTexCoord2f(0, 1); glVertex3f( -0.5f,  0.5f, -0.5f );
  glEnd();
  
  // Right
  glBindTexture(GL_TEXTURE_2D, textures[TEXTUREID_CHECKER_GREEN]);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( 0.5f, -0.5f, -0.5f );
    glTexCoord2f(1, 0); glVertex3f( 0.5f, -0.5f,  0.5f );
    glTexCoord2f(1, 1); glVertex3f( 0.5f,  0.5f,  0.5f );
    glTexCoord2f(0, 1); glVertex3f( 0.5f,  0.5f, -0.5f );
  glEnd();
  
  // Back
  glBindTexture(GL_TEXTURE_2D, textures[TEXTUREID_CHECKER_YELLOW]);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( -0.5f, -0.5f, 0.5f );
    glTexCoord2f(1, 0); glVertex3f(  0.5f, -0.5f, 0.5f );
    glTexCoord2f(1, 1); glVertex3f(  0.5f,  0.5f, 0.5f );
    glTexCoord2f(0, 1); glVertex3f( -0.5f,  0.5f, 0.5f );
  glEnd();
  
  // Top
  glBindTexture(GL_TEXTURE_2D, textures[TEXTUREID_CHECKER_PURPLE]);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( -0.5f,  0.5f, -0.5f );
    glTexCoord2f(1, 0); glVertex3f(  0.5f,  0.5f, -0.5f );
    glTexCoord2f(1, 1); glVertex3f(  0.5f,  0.5f, 0.5f );
    glTexCoord2f(0, 1); glVertex3f( -0.5f,  0.5f, 0.5f );
  glEnd();
  
  // Top
  glBindTexture(GL_TEXTURE_2D, textures[TEXTUREID_CHECKER_CYAN]);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( -0.5f,  -0.5f, -0.5f );
    glTexCoord2f(1, 0); glVertex3f(  0.5f,  -0.5f, -0.5f );
    glTexCoord2f(1, 1); glVertex3f(  0.5f,  -0.5f, 0.5f );
    glTexCoord2f(0, 1); glVertex3f( -0.5f,  -0.5f, 0.5f );
  glEnd();
  
  // Restore world matrix
  glPopMatrix();
}


/**
 * Initialize OpenGL
 */
void opengl_init()
{
  glEnable(GL_DEPTH_TEST);

  //Initialize Projection Matrix
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  //Initialize Modelview Matrix
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  //Initialize clear color
  glClearColor( 0.f, 0.f, 0.f, 1.f );

  //Check for error
  GLenum error = glGetError();
  if(error != GL_NO_ERROR )
  {
    SDL_Quit();
    exit(EXIT_FAILURE);
  }
  
  glEnable(GL_TEXTURE_2D);
}


/**
 * Init SDL and all subsystems
 */
void sdl_init(char* caption)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0 )
    exit(EXIT_FAILURE);

  if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_OPENGL)))
  {
    SDL_Quit();
    exit(EXIT_FAILURE);
  }
  
  SDL_WM_SetCaption(caption, NULL );
}


int main(int argc, char** argv)
{
  SDL_Event event;

  int keypress = 0;
  int t = 0;
  
  // Init everything
  sdl_init("OpenGL example");
  opengl_init();
  
  init_sin_table();
  init_textures();
  
  // Init random number generator
  srand(clock());
  
  // Loop until the end
  while(!keypress) 
  {
    // Lock SDL surface if needed
    if(SDL_MUSTLOCK(screen)) 
      if(SDL_LockSurface(screen) < 0)
	exit(EXIT_FAILURE);
  
    // Draw the plasma
    draw_cube(screen, t);

    // Unlock the SDL surface if needed
    if(SDL_MUSTLOCK(screen))
      SDL_UnlockSurface(screen);
  
    // Update screen
    SDL_GL_SwapBuffers();
      
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
