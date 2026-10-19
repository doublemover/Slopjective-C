#include "carrier-array-call.h"
#if CASE == 1
void rejected(void) { CallStrong values[2]; (void)values; }
#elif CASE == 2
void rejected(void) { __block CallStrong values[2] = {call_make(0)}; (void)values; }
#elif CASE == 3
struct __attribute__((packed)) Container { char c; CallStrong values[2]; };
void rejected(void) { struct Container value = {0,{call_make(0)}}; (void)value; }
#endif
