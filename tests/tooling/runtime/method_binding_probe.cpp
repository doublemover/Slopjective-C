#include "method_binding_probe/main_orchestration.h"

int main() {
  namespace probe = objc3c::tooling::method_binding_probe;
  return probe::RunMethodBindingProbe();
}
