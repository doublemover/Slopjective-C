#include "runtime_fast_path_contract_probe/main_orchestration.h"

int main() {
  namespace probe = objc3c::tooling::runtime_fast_path_contract_probe;
  return probe::RunRuntimeFastPathContractProbe();
}
