#include "live_dispatch_fast_path_probe/main_orchestration.h"

int main() {
  namespace probe = objc3c::tooling::live_dispatch_fast_path_probe;
  return probe::RunLiveDispatchFastPathProbe();
}
