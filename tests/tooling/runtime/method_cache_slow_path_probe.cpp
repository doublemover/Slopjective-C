#include "method_cache_slow_path_probe/main_orchestration.h"

int main() {
  namespace probe = objc3c::tooling::method_cache_slow_path_probe;
  return probe::RunMethodCacheSlowPathProbe();
}
