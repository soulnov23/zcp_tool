#pragma once

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

#ifndef LIKELY
#if defined(__GNUC__) && __GNUC__ >= 4
#define LIKELY(x) (__builtin_expect((x), 1))
#else
#define LIKELY(x) (x)
#endif
#endif

#ifndef UNLIKELY
#if defined(__GNUC__) && __GNUC__ >= 4
#define UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define UNLIKELY(x) (x)
#endif
#endif

#define LIBCO_NONCOPYABLE(T)         \
  void operator=(const T&) = delete; \
  T(const T&) = delete;

#define LIBCO_CACHELINE_SIZE 64
#define LIBCO_CACHELINE_ALIGNMENT __attribute__((aligned(LIBCO_CACHELINE_SIZE)))

namespace libco {

inline void asm_volatile_pause() { asm volatile("pause"); }

}  // namespace libco
