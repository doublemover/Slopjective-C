#pragma once

enum class Objc3DriverStatusCode : int {
  kSuccess = 0,
  kDiagnosticsPresent = 1,
  kInputUnavailable = 2,
  kNativeToolchainFailure = 3,
  kHardCutoverContractFailure = 125,
};

constexpr int Objc3DriverStatusValue(Objc3DriverStatusCode code) {
  return static_cast<int>(code);
}

const char *Objc3DriverStatusCodeName(Objc3DriverStatusCode code);
