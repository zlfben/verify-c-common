#include <bounds.h>
#include <seahorn/seahorn.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define INLINE __attribute__((always_inline))
/**
  The bcmp() function compares the two byte sequences s1 and s2 of
  length n each.  If they are equal, and in particular if n is zero,
  bcmp() returns 0.  Otherwise, it returns a nonzero result.
*/
INLINE int bcmp(const void *s1, const void *s2, size_t n) {
  size_t i;

  size_t max_buffer_size = sea_max_buffer_size();
  const uint8_t *p1;
  const uint8_t *p2;
  p1 = s1;
  p2 = s2;

  if (p1 == p2)
    return 0;
  if (p1 == NULL || p2 == NULL)
    return 1;
  /* pre-unroll the loop for MAX_BUFFER_SIZE */
  for (i = 0; i < max_buffer_size; i++) {
    if (i < n) {
      if (p1[i] != p2[i]) {
        return 1;
      }
    }
  }
  /* unroll the rest, if any */
  for (i = max_buffer_size; i < n; i++) {
    if (p1[i] != p2[i])
      return 1;
  }

  return 0;
}

INLINE int memcmp(const void *s1, const void *s2, size_t n) {
  size_t i;
  size_t max_buffer_size = sea_max_buffer_size();

  const uint8_t *p1;
  const uint8_t *p2;
  p1 = s1;
  p2 = s2;

  if (p1 == p2)
    return 0;

  /* pre-unroll the loop for MAX_BUFFER_SIZE */
  for (i = 0; i < max_buffer_size; i++) {
    if (i < n) {
      if (p1[i] != p2[i]) {
        return p1[i] < p2[i] ? -1 : 1;
      }
    }
  }
  /* unroll the rest, if any */
  for (i = max_buffer_size; i < n; i++) {
    if (p1[i] != p2[i])
      return p1[i] < p2[i] ? -1 : 1;
  }

  return 0;
}

void *memcpy_impl(void *dst, const void *src, size_t n) {
    for (size_t i = 0; i < n; ++i)
        ((char *)dst)[i] = ((const char *)src)[i];
    return dst;
}

INLINE void *__memcpy_chk(void *dest, const void *src, size_t len,
                          size_t dstlen) {
  sassert(!(dstlen < len));
  return memcpy_impl(dest, src, len);
}

void *memcpy(void *dst, const void *src, size_t n) {
  return memcpy_impl(dst, src, n);
}

void *memset_impl(void *s, int c, size_t n) {
    if (n > 0) {
        unsigned char *sp = (unsigned char *)s;
        for (size_t i = 0; i < n; i++)
            sp[i] = c & UINT8_MAX;
    }
    return s;
}

INLINE void *__memset_chk(void *dest, int c, size_t len, size_t dstlen) {
  sassert(!(dstlen < len));
  return memset_impl(dest, c, len);
}

void *memset(void *dest, int c, size_t len) {
  return memset_impl(dest, c, len);
}

void *memmove_impl(void *dest, const void *src, size_t n) {
    if (n > 0) {
        (void)*(char *)dest;                           /* check that the memory is accessible */
        (void)*(const char *)src;                      /* check that the memory is accessible */
        (void)*(((unsigned char *)dest) + n - 1);      /* check that the memory is accessible */
        (void)*(((const unsigned char *)src) + n - 1); /* check that the memory is accessible */

        unsigned char *pd = dest;
        const unsigned char *ps = src;
        if ((ps) < (pd)) {
            for (pd += n, ps += n; n--;)
                *--pd = *--ps;
        } else {
            while (n) {
                *pd++ = *ps++;
                n--;
            }
        }
    }
    return dest;
}

void *memmove(void *dest, const void *src, size_t n) {
  return memmove_impl(dest, src, n);
}

// /* on OSX memmove is a macro */
// #ifndef memmove
// INLINE void* memmove(void *dst, const void *src, size_t len)  {
//   sassert(sea_is_dereferenceable(dst, len));
//   sassert(sea_is_dereferenceable(src, len));
//   return __builtin_memmove(dst, src, len);
// }
// #endif

// /* on OSX memcpy is a macro */
// #ifndef memcpy
// INLINE void *memcpy(void *dst, const void *src, size_t len) {
//   sassert(sea_is_dereferenceable(dst, len));
//   sassert(sea_is_dereferenceable(src, len));
//   return memcpy_impl(dst, src, len);
// }
// #endif
