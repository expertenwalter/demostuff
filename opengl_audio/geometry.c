#include "geometry.h"


// Precalculated values
static float sintable[360];


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
