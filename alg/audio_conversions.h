
#ifndef AUDIO_CONVERSIONS_H
#define AUDIO_CONVERSIONS_H

typedef struct {
  char val[3];
  void operator=(const __int32 i) {
    val[0] = i & 0xff;
    val[1] = i & 0xff00;
    val[2] = i & 0xff0000;
  }
} __int24;

#endif//AUDIO_CONVERSIONS_H
