#pragma once

#ifdef _WIN32
	#define INLINE __forceinline
#else
	#define INLINE __attribute__((always_inline))
#endif