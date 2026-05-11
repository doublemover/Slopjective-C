#include "ownership_runtime_hook_probe/main_orchestration.h"

namespace probe = objc3c::runtime::probe::ownership_runtime_hook;

int main() {
  return probe::RunOwnershipRuntimeHookProbe();
}
