#include "runtime/public/objc3_runtime_result_ownership.h"

#include "runtime/metadata/runtime_ownership_contracts.h"

namespace objc3c::runtime {

const char *RuntimeResultDiagnosticOwnerModel() {
  return kObjc3RuntimePublicDispatchDiagnosticsOwner;
}

const char *RuntimeResultFailClosedOwnershipModel() {
  return kObjc3RuntimeFailClosedOwnershipModel;
}

}  // namespace objc3c::runtime
