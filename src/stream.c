#include "stream.h"

#include <stdbool.h>
#include <stddef.h>

void init_cache(Stream* stream) {
	stream->cache.cardinalOfW.present = false;
	stream->cache.cardinalOfT.present = false;
	stream->cache.cardinalOfE.present = false;
	stream->cache.cardinalOfV.present = false;
}