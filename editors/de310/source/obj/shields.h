// SHIELDS.H - info for items of type ITEM_SHIELD

#ifndef _SHIELDS_H_

// type (val0)

#define SHIELDTYPE_LOWEST   1
#define SHIELDTYPE_STRAPARM 1  // strapped to the arm - bucklers, others
#define SHIELDTYPE_HANDHELD 2  // held by hand
#define SHIELDTYPE_HIGHEST  2

// shape (val1)

#define SHIELDSHAPE_LOWEST   1
#define SHIELDSHAPE_CIRCULAR 1  // perfect circle
#define SHIELDSHAPE_SQUARE   2  // square..
#define SHIELDSHAPE_RECTVERT 3  // a rectangle aligned vertically
#define SHIELDSHAPE_RECTHORZ 4  // horizontally..  you never know
#define SHIELDSHAPE_OVALVERT 5  // vertical 'oval'
#define SHIELDSHAPE_OVALHORZ 6  // horizontal 'oval' - you never know
#define SHIELDSHAPE_TRIBIGUP 7  // triangle - wide side on top
#define SHIELDSHAPE_TRISMLUP 8  // triangle - narrow point on top

// dunno what they're called, but the type of shield that is square on
// top and rounded on the bottom should be added

#define SHIELDSHAPE_HIGHEST  8

// size (val2) - used to determine how well it can block (and how well it
//               encumbers your weapon use, perhaps)

#define SHIELDSIZE_LOWEST   1
#define SHIELDSIZE_TINY     1  // really small suckers
#define SHIELDSIZE_SMALL    2  // bucklers, small shields
#define SHIELDSIZE_MEDIUM   3  // normal shields
#define SHIELDSIZE_LARGE    4  // big shields
#define SHIELDSIZE_HUGE     5  // huge shields (might not need this)
#define SHIELDSIZE_HIGHEST  5

#define _SHIELDS_H_
#endif
