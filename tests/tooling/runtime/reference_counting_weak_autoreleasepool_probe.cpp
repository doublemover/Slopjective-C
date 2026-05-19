#include "reference_counting_weak_autoreleasepool_probe/main_orchestration.h"

int main() {
  namespace probe =
      objc3c::runtime::probe::reference_counting_weak_autoreleasepool;
  return probe::RunReferenceCountingWeakAutoreleasepoolProbe();
}
