#include "exception_adapter.h"
#include <exception>
struct Guard { ~Guard() { adapter_cleaned(); } };
void adapter_throw_cpp(int key, int mode) {
  if (mode == 1) {
    @try { Guard inner; @throw adapter_make(key); }
    @catch (id error) { Guard outer; if (!error) __builtin_trap(); @throw; }
  } else if (mode == 2) {
    Guard guard;
    adapter_throw_c(key);
  } else if (mode == 3) {
    Guard guard;
    throw adapter_make(key);
  } else {
    Guard guard;
    @throw adapter_make(key);
  }
}
int adapter_uncaught(void) { return std::uncaught_exceptions(); }
