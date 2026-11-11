#include <cassert>

// CTest WILL_FAIL must observe an ordinary nonzero exit. If assertions are
// removed, this control exits successfully and CTest rejects the configuration.
int main() {
  int checks = 0;
  assert(++checks == 1);
  return checks == 1 ? 1 : 0;
}
