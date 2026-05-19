#include "driver/objc3_driver_status_codes.h"

const char *Objc3DriverStatusCodeName(Objc3DriverStatusCode code) {
  switch (code) {
    case Objc3DriverStatusCode::kSuccess:
      return "success";
    case Objc3DriverStatusCode::kDiagnosticsPresent:
      return "diagnostics-present";
    case Objc3DriverStatusCode::kInputUnavailable:
      return "input-unavailable";
    case Objc3DriverStatusCode::kNativeToolchainFailure:
      return "native-toolchain-failure";
    case Objc3DriverStatusCode::kHardCutoverContractFailure:
      return "hard-cutover-contract-failure";
  }
  return "unknown";
}
