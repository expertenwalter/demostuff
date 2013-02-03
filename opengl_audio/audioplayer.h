#ifndef _AUDIOPLAYER_H_
#define _AUDIOPLAYER_H_

typedef struct {
  int len;
  float freq;
} audioplayer_note_t;

typedef struct {
  int bpm;
  int numnotes;
  audioplayer_note_t notes[];
} audioplayer_tune_t;


void audioplayer_init();

void audioplayer_loadtune(audioplayer_tune_t* data);

void audioplayer_play();

float audioplayer_getpos();

#endif
