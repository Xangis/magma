// GRAPHCON.H - constants for non-graph.h usin' freaks

#ifndef _GRAPHCON_H_

#if (!defined(_WIN32) && !defined(__UNIX__))
#  error graphcon.h should only be used for non-DOS crap
#endif

#ifdef _WIN32
#include <mapiutil.h>
#include <mapiwin.h>
#include <wincon.h>
#include "types.h"

struct rccoord
{
  SHORT row;
  SHORT col;
};
#endif

#ifdef __UNIX__
struct rccoord
{
  int row;
  int col;
};
#endif

#define _GRAPHCON_H_
#endif
