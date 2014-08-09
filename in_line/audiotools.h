
#ifndef AUDIOTOOLSH
#define AUDIOTOOLSH

#include "../alg/audio_conversions.h"

template<typename T>
void Remap(T* dest, size_t dest_nch, T* src, size_t src_nch, size_t chnmap[], int numsamples) {
  T* const dest_end = dest + numsamples*dest_nch;
  while(dest != dest_end) {
    for(size_t i = 0; i < dest_nch; ++i) {
      const size_t src_chn = chnmap[i];
      if ( src_chn == -1 ) {
        *dest++ = 0;
      }
      else {
        *dest++ = src[src_chn];
      }
    }
    src += src_nch;
  }
}

void Remap(void* dest_p, size_t dest_nch, void* src_p, size_t src_nch, size_t chnmap[], size_t bps, int numsamples)
{
	switch(bps) {
	case 8:
		{
      // 0.022
      __int8* dest = reinterpret_cast<__int8*>(dest_p);
      __int8* src = reinterpret_cast<__int8*>(src_p);
      Remap(dest, dest_nch, src, src_nch, chnmap, numsamples);
		}
		break;
  case 16:
		{
      // 0.030
      __int16* dest = reinterpret_cast<__int16*>(dest_p);
      __int16* src = reinterpret_cast<__int16*>(src_p);
      Remap(dest, dest_nch, src, src_nch, chnmap, numsamples);
		}
		break;
  case 24:
		{
      // 0.045
      __int24* dest = reinterpret_cast<__int24*>(dest_p);
      __int24* src = reinterpret_cast<__int24*>(src_p);
      Remap(dest, dest_nch, src, src_nch, chnmap, numsamples);
		}
		break;
  case 32:
		{
      // 0.056
      __int32* dest = reinterpret_cast<__int32*>(dest_p);
      __int32* src = reinterpret_cast<__int32*>(src_p);
      Remap(dest, dest_nch, src, src_nch, chnmap, numsamples);
		}
		break;
  }
	return;
}

#endif // AUDIOTOOLSH
