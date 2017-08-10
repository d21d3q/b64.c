
/**
 * `encode.c' - b64
 *
 * copyright (c) 2014 joseph werle
 */

#include <stdio.h>
#include <stdlib.h>
#include "b64.h"

#ifdef b64_USE_CUSTOM_MALLOC
extern void* b64_malloc(size_t);
#endif

#ifdef b64_USE_CUSTOM_REALLOC
extern void* b64_realloc(void*, size_t);
#endif

static char *
encode (const unsigned char *src, size_t len, char *obuf, size_t *obuf_size) {
  int i = 0;
  int j = 0;
  char *enc = NULL;
  size_t size = 0;
  unsigned char buf[4];
  unsigned char tmp[3];

  if (NULL == obuf){
    // alloc
    enc = (char *) b64_malloc(1);
    if (NULL == enc) { return NULL; }
  } else {
    if (NULL != obuf_size){
      if (*obuf_size < 4 * ((len + 2) / 3) + 1){
        return NULL;
      } else {
        enc = obuf;
      } 
    } else {
      return NULL;
    }
  }

  // parse until end of source
  while (len--) {
    // read up to 3 bytes at a time into `tmp'
    tmp[i++] = *(src++);

    // if 3 bytes read then encode into `buf'
    if (3 == i) {
      buf[0] = (tmp[0] & 0xfc) >> 2;
      buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
      buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
      buf[3] = tmp[2] & 0x3f;

      if (NULL == obuf){
        // if we are working on dynamic buffer
        // allocate 4 new byts for `enc` and
        enc = (char *) b64_realloc(enc, size + 4);
        if (NULL == enc) { return NULL; }
      }

      // then translate each encoded buffer
      // part by index from the base 64 index table
      // into `enc' unsigned char array
      for (i = 0; i < 4; ++i) {
        enc[size++] = b64_table[buf[i]];
      }

      // reset index
      i = 0;
    }
  }

  // remainder
  if (i > 0) {
    // fill `tmp' with `\0' at most 3 times
    for (j = i; j < 3; ++j) {
      tmp[j] = '\0';
    }

    // perform same codec as above
    buf[0] = (tmp[0] & 0xfc) >> 2;
    buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
    buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
    buf[3] = tmp[2] & 0x3f;

    // perform same write to `enc` with new allocation
    for (j = 0; (j < i + 1); ++j) {
      if (NULL == obuf){
        enc = (char *) b64_realloc(enc, size + 1);
        if (NULL == enc) { return NULL; }
      }
      enc[size++] = b64_table[buf[j]];
    }

    // while there is still a remainder
    // append `=' to `enc'
    while ((i++ < 3)) {
      if (NULL == obuf){
        enc = (char *) b64_realloc(enc, size + 1);
        if (NULL == enc) { return NULL; }
      }
      enc[size++] = '=';
    }
  }

  // Make sure we have enough space to add '\0' character at end.
  if (NULL == obuf){
    enc = (char *) b64_realloc(enc, size + 1);
    if (NULL == enc) { return NULL; }
  }
  enc[size] = '\0';
  if (NULL != obuf_size){
    *obuf_size = size;
  }

  return enc;
}

char *
b64_encode (const unsigned char *src, size_t len) {
  return encode(src, len, NULL, NULL);
}

char *
b64_encode_static (const unsigned char *src, size_t len, unsigned char *dst, size_t *size){
  return encode(src, len, dst, size);
}
