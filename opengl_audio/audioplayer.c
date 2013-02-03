#include <stdint.h>

#include <SDL/SDL.h>

#include "geometry.h"

#include "audioplayer.h"

static audioplayer_tune_t* tune;
static long playerposition;
static char playing = 0;
static int tuneposition = 0;
static int beatsplayed;

static const int AUDIO_SAMPLERATE = 44100;

static void fill_audio(void *udata, Uint8 *stream, int len)
{
  memset(stream, 0, len);
  
  if(!playing)
    return;
  
  int16_t* data = (int16_t*) stream;
  
  for(int i = 0; i < (len / 4) && playing; i++)
  {
    // Check if a new beat begins in this sample
    if((playerposition + i) % ((AUDIO_SAMPLERATE * 60) / tune->bpm) == 0)
    {
      beatsplayed++;
      
      // Check if the current note in the tune is over
      if(beatsplayed >= tune->notes[tuneposition].len)
      {
	tuneposition++;
	beatsplayed = 0;
	
	// Check if the tune is over
	if(tuneposition > tune->numnotes)
	{
	  playing = 0;
	  continue;
	}
      }
    }
    
    // Generate some noise
    float freq = tune->notes[tuneposition].freq;
    
    data[0] = 32767 * fast_sin(freq * ((playerposition + i) / (float)AUDIO_SAMPLERATE) * 2 * MATH_PI);
    data[1] = data[0];
    data += 2;
  }
  
  playerposition += len / 4;
}

void audioplayer_init()
{
  SDL_AudioSpec wanted;
  
  /* Set the audio format */
  wanted.freq = AUDIO_SAMPLERATE;
  wanted.format = AUDIO_S16LSB;
  wanted.channels = 2;    /* 1 = mono, 2 = stereo */
  wanted.samples = 512;  /* Good low-latency value for callback */
  wanted.callback = fill_audio;
  wanted.userdata = NULL;
  
  SDL_OpenAudio(&wanted, NULL);
}

void audioplayer_loadtune(audioplayer_tune_t* data)
{
  tune = data;
}


void audioplayer_play()
{
  tuneposition = 0;
  playerposition = 0;
  playing = 1;
  beatsplayed = 0;
  
  SDL_PauseAudio(0);
}

float audioplayer_getpos()
{
  return (playerposition / 44100.f);
}

char audioplayer_isplaying()
{
  return playing;
}