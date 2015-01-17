#pragma once

// Define some common things of MiniSTL
// in this file
//

#ifdef NO_NS

#define START_MINISTL
#define END_MINISTL
#define USE_MINISTL

#else

#define START_MINISTL namespace ministl {
#define END_MINISTL }
#define USE_MINISTL using namespace ministl

#endif

