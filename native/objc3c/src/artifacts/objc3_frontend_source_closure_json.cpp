#include "artifacts/objc3_frontend_source_closure_artifacts.h"

#include <sstream>
#include <string>
#include <vector>

#include "io/json/json_writer.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  return objc3::io::json::RenderJsonStringArray(values);
}

}  // namespace

#include "artifacts/objc3_frontend_source_closure_type_control_error_json.inc"
#include "artifacts/objc3_frontend_source_closure_dispatch_json.inc"
#include "artifacts/objc3_frontend_source_closure_metaprogramming_json.inc"
#include "artifacts/objc3_frontend_source_closure_interop_json.inc"

}  // namespace objc3::artifacts::frontend
