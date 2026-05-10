#pragma once

#include <string>

namespace objc3::artifacts::frontend::type_system_contract_artifacts {

inline std::string BoolToken(bool value) { return value ? "true" : "false"; }

}  // namespace objc3::artifacts::frontend::type_system_contract_artifacts
