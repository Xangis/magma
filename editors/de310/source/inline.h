#ifndef _INLINE_H_

#ifdef _WIN32
#  define INLINEDEF inline
#  define INLINEDEC extern inline
#else
#  define INLINEDEF /*inline*/
#  define INLINEDEC /*extern inline*/
#endif

#define _INLINE_H_
#endif
