
/**
 * `test.c' - b64
 *
 * copyright (c) 2014 joseph werle
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ok/ok.h>

#include "b64.h"

#define S(x) # x
#define t(m, a, b) ({                                                \
    char tmp[1024];                                                  \
    sprintf(tmp, "%s(%s) = %s", S(m), S(a), S(b));                   \
    char *r = (char *) m(a, strlen((char *) a));                     \
    assert(0 == strcmp(b, r));                                       \
    free(r);                                                         \
    ok(tmp);                                                         \
})

// macro for testing functions with buffers
#define t_buf(m, buf, len, a, b) do{                                 \
    char tmp[1024];                                                  \
    sprintf(tmp, "%s(%s) = %s", S(m), S(a), S(b));                   \
    char *r = (char *) m(a, strlen((char *) a), buf, len);           \
    assert(NULL != r);                                               \
    assert(0 == strcmp(b, r));                                       \
    ok(tmp);                                                         \
} while (0)

// test if function returns null for given parameters
#define t_buf_fail(m, buf, len, a, b) do{                            \
    char tmp[1024];                                                  \
    sprintf(tmp, "%s(%s) = %s", S(m), S(a), S(b));                   \
    char *r = (char *) m(a, strlen((char *) a), buf, len);           \
    assert(NULL == r);                                               \
    free(r);                                                         \
    ok(tmp);                                                         \
} while (0)

#define BUF1_SIZE 1024
#define BUF2_SIZE 8
#define BUF3_size 9

void* custom_malloc(size_t size){
  if (size == 0){
    /* On some systems malloc doesn't allow for size = 0 */
    return NULL;
  }
  return malloc(size);
}

void* custom_realloc(void* ptr, size_t size){
  return realloc(ptr, size);
}

int
main (void) {

  // encode
  {
    t(b64_encode, (const unsigned char *) "bradley", "YnJhZGxleQ==");
    t(b64_encode, (const unsigned char *) "kinkajou", "a2lua2Fqb3U=");
    t(b64_encode, (const unsigned char *) "vino", "dmlubw==");
    t(b64_encode,
        (const unsigned char *) "brian the monkey and bradley the kinkajou are friends",
        "YnJpYW4gdGhlIG1vbmtleSBhbmQgYnJhZGxleSB0aGUga2lua2Fqb3UgYXJlIGZyaWVuZHM=");
  }

  // static buffer tests
  {
    unsigned char buf_big[BUF1_SIZE];
    size_t size = BUF1_SIZE;
    t_buf(b64_encode_static, buf_big, &size, (const unsigned char *) "bradley", "YnJhZGxleQ==");
    size = BUF1_SIZE;
    t_buf(b64_encode_static, buf_big, &size, (const unsigned char *) "kinkajou", "a2lua2Fqb3U=");
    size = BUF1_SIZE;
    t_buf(b64_encode_static, buf_big, &size, (const unsigned char *) "vino", "dmlubw==");
    size = BUF1_SIZE;
    t_buf(b64_encode_static, buf_big, &size,
        (const unsigned char *) "brian the monkey and bradley the kinkajou are friends",
        "YnJpYW4gdGhlIG1vbmtleSBhbmQgYnJhZGxleSB0aGUga2lua2Fqb3UgYXJlIGZyaWVuZHM=");
    
    // too small output buffers
    // "kinkajou" would need 12 bytes for encoded data and 1 for terminating \0
    // so lets try with buff qith exact size
    size = 13;
    t_buf(b64_encode_static, buf_big, &size, (const unsigned char *) "kinkajou", "a2lua2Fqb3U=");
    assert(size == 12);
    // and with buffer which is too small
    size = 12;
    t_buf_fail(b64_encode_static, buf_big, &size, (const unsigned char *) "kinkajou", "a2lua2Fqb3U=");
  }

  // decode
  {
    t(b64_decode, "Y2FzaWxsZXJv", "casillero");
    t(b64_decode, "aGF4", "hax");
    t(b64_decode, "bW9ua2V5cyBhbmQgZG9ncw==", "monkeys and dogs");
    t(b64_decode,
        "dGhlIGtpbmtham91IGFuZCBtb25rZXkgZm91Z2h0IG92ZXIgdGhlIGJhbmFuYQ==",
        "the kinkajou and monkey fought over the banana");
  }

  ok_done();
  return 0;
}
