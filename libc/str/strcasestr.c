/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/str/str.h"
#include "libc/mem/alloca.h"
#include "libc/runtime/stack.h"
#include "libc/str/tab.h"

static void computeLPS(const char *pattern, long M, long *lps) {
  long len = 0;
  lps[0] = 0;
  long i = 1;
  while (i < M) {
    if (kToLower[pattern[i] & 255] == kToLower[pattern[len] & 255]) {
      len++;
      lps[i] = len;
      i++;
    } else {
      if (len != 0) {
        len = lps[len - 1];
      } else {
        lps[i] = 0;
        i++;
      }
    }
  }
}

static char *kmp(const char *s, size_t n, const char *ss, size_t m) {
  if (!m)
    return (char *)s;
  if (n < m)
    return NULL;
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Walloca-larger-than="
#pragma GCC diagnostic ignored "-Wanalyzer-out-of-bounds"
  long need = sizeof(long) * m;
  long *lps = (long *)alloca(need);
  CheckLargeStackAllocation(lps, need);
#pragma GCC pop_options
  computeLPS(ss, m, lps);
  long i = 0;
  long j = 0;
  while (i < n) {
    if (kToLower[ss[j] & 255] == kToLower[s[i] & 255]) {
      i++;
      j++;
    }
    if (j == m) {
      return (char *)(s + i - j);
    } else if (i < n && kToLower[ss[j] & 255] != kToLower[s[i] & 255]) {
      if (j != 0) {
        j = lps[j - 1];
      } else {
        i++;
      }
    }
  }
  return NULL;
}

/**
 * Searches for substring case-insensitively.
 *
 * @param haystack is the search area, as a NUL-terminated string
 * @param needle is the desired substring, also NUL-terminated
 * @return pointer to first substring within haystack, or NULL
 * @asyncsignalsafe
 * @see strstr()
 */
char *strcasestr(const char *haystack, const char *needle) {
  return kmp(haystack, strlen(haystack), needle, strlen(needle));
}
