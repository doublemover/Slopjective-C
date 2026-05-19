#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "artifacts/objc3_frontend_interop_semantic_artifacts.h"
#include "io/json/json_writer.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend::interop_semantic_json_detail {

using objc3::io::EscapeJsonString;

inline std::string BuildStringArrayJson(
    const std::vector<std::string> &values) {
  return objc3::io::json::RenderJsonStringArray(values);
}

}  // namespace objc3::artifacts::frontend::interop_semantic_json_detail
