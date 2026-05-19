#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_IMAGE_FIXTURE_SETUP_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_IMAGE_FIXTURE_SETUP_H_

#include "../support/json_probe_writer.h"

#include <string>

namespace objc3c::runtime::probe::multi_image_registration_reset_replay {

inline constexpr const char *kImportedProviderClassName = "ImportedProvider";
inline constexpr const char *kLocalConsumerClassName = "LocalConsumer";

inline std::string CopyRuntimeString(const char *value) {
  return objc3c::runtime::probe::CopyJsonString(value);
}

inline const char *NullableRuntimeString(const std::string &value) {
  return objc3c::runtime::probe::NullableCString(value);
}

}  // namespace objc3c::runtime::probe::multi_image_registration_reset_replay

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_IMAGE_FIXTURE_SETUP_H_
