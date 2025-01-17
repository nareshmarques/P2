#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "vad.h"
#include "pav_analysis.h"

const float FRAME_TIME = 10.0F; /* in ms. */

/* 
 * As the output state is only ST_VOICE, ST_SILENCE, or ST_UNDEF,
 * only this labels are needed. You need to add all labels, in case
 * you want to print the internal state in string format
 */

const char *state_str[] = {
  "UNDEF", "S", "V", "INIT"
};

const char *state2str(VAD_STATE st) {
  return state_str[st];
}

/* Define a datatype with interesting features */
typedef struct {
  float zcr;
  float p;
  float am;
} Features;

/* 
 * TODO: Delete and use your own features!
 */

Features compute_features(const float *x, int N) {
  
  Features feat;
  feat.zcr = compute_zcr(x, N, 16000); // vad_data->sampling_rate
  feat.p = compute_power(x, N);
  feat.am = compute_am(x, N);
  return feat;
}

/* 
 * TODO: Init the values of vad_data
 */

VAD_DATA * vad_open(float rate, float alfa_1, float alfa_2, int count_ms, int count_mv) {
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->lastState = ST_UNDEF;
  vad_data->sampling_rate = rate;
  vad_data->frame_length = rate * FRAME_TIME * 1e-3;
  vad_data->alfa_1 = alfa_1;
  vad_data->alfa_2 = alfa_2;
  vad_data->count_ms = count_ms;
  vad_data->count_mv = count_mv;
  vad_data->count_initFrames = 7;  
  vad_data->count_undefined = 0;
  vad_data->count_initial = 0;
  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data) {
  /* 
   * TODO: decide what to do with the last undecided frames
   */
  VAD_STATE state = vad_data->state;

  free(vad_data);
  return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data) {
  return vad_data->frame_length;
}

/* 
 * TODO: Implement the Voice Activity Detection 
 * using a Finite State Automata
 */

VAD_STATE vad(VAD_DATA *vad_data, float *x) {

  /* 
   * TODO: You can change this, using your own features,
   * program finite state automaton, define conditions, etc.
   */

  Features f = compute_features(x, vad_data->frame_length);
  vad_data->last_feature = f.p; /* save feature, in case you want to show */

  switch (vad_data->state) {
    case ST_INIT: if(vad_data->count_initial < vad_data->count_initFrames){
                    vad_data->count_initial = vad_data->count_initial + 1;
                    vad_data->zcr0 = vad_data->zcr0 + f.zcr;
                    vad_data->p0 = vad_data->p0 + pow(10, f.p/10);
                  } else{
                    vad_data->zcr0 = (vad_data->zcr0) / vad_data->count_initial;
                    vad_data->p0 = 10*log10((vad_data->p0) / vad_data->count_initFrames);
                    vad_data->k1 = vad_data->p0 + vad_data->alfa_1;
                    vad_data->k2 = vad_data->k1 + vad_data->alfa_2;
                    vad_data->state = ST_SILENCE;
                  }
                  vad_data->lastState = ST_INIT;
                  break;

    case ST_SILENCE:  vad_data->count_undefined = 0;
                      if(f.p > vad_data->k1){
                        vad_data->state = ST_MAYBEVOICE;
                      } else{
                        vad_data->state = ST_SILENCE;
                      }
                      vad_data->lastState = ST_SILENCE;
                      break;

    case ST_VOICE:  vad_data->count_undefined = 0;
                    if((f.p < vad_data->k1) && (f.zcr < vad_data->zcr0)){ // (f.p < vad_data->k1) && (f.zcr < vad_data->zcr0)
                      vad_data->state = ST_MAYBESILENCE;
                    }
                    vad_data->lastState = ST_VOICE;
                    break;

    case ST_MAYBESILENCE: vad_data->count_undefined = vad_data->count_undefined + 1;
                          if(f.p < vad_data->p0){ // P0 o k1????
                            vad_data->state = ST_SILENCE;
                          }
                          if(f.p > vad_data->k2){
                            vad_data->state = ST_VOICE;
                          }
                          if((f.p > vad_data->k1) && (vad_data->count_undefined > vad_data->count_ms)){ // (f.p > vad_data->k1) && (vad_data->count_undefined > vad_data->count_ms)
                            vad_data->state = ST_SILENCE;
                          }
                          vad_data->lastState = ST_MAYBESILENCE;
                          break;

    case ST_MAYBEVOICE: vad_data->count_undefined = vad_data->count_undefined + 1;
                        if(f.p > vad_data->k2){
                          vad_data->state = ST_VOICE;
                        }
                        if((f.p > vad_data->k1) && (vad_data->count_undefined > vad_data->count_mv)){ // (f.p > vad_data->k1) && (vad_data->count_undefined > vad_data->count_mv)
                          vad_data->state = ST_VOICE;
                        }
                        if(f.p < vad_data->k1){
                          vad_data->state = ST_SILENCE;
                        }
                        vad_data->lastState = ST_MAYBEVOICE;
                        break;

    case ST_UNDEF:  break;
  }
  
  if (vad_data->state == ST_SILENCE || vad_data->state == ST_VOICE){
    return vad_data->state;
  } else{
    return ST_UNDEF;
  }
}


void vad_show_state(const VAD_DATA *vad_data, FILE *out) {
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}
