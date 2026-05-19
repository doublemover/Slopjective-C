#pragma once

#include <string>

static inline bool FailRuntimeMetadataLayoutPolicyBuild(
    Objc3RuntimeMetadataLayoutPolicy &policy, std::string &error,
    const std::string &message) {
  error = message;
  policy.failure_reason = error;
  return false;
}
