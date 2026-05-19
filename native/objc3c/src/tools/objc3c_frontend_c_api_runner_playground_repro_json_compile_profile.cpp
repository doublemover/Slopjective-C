#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_compile_profile.h"

#include <ostream>

#include "io/objc3_json.h"
#include "libobjc3c_frontend/c_api.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerPlaygroundReproCompileProfileJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerOptions &options) {
  const char *backend_name =
      options.ir_object_backend == OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT
          ? "llvm-direct"
          : "clang";
  out << child_indent << "\"compile_profile\": {\n";
  out << grandchild_indent << "\"ir_object_backend\": \"" << backend_name
      << "\",\n";
  out << grandchild_indent << "\"max_message_send_args\": "
      << options.max_message_send_args << ",\n";
  out << grandchild_indent << "\"runtime_dispatch_symbol\": \""
      << EscapeJsonString(options.runtime_dispatch_symbol) << "\",\n";
  out << grandchild_indent << "\"allow_live_error_runtime_surface\": "
      << (options.allow_live_error_runtime_surface ? "true" : "false")
      << ",\n";
  out << grandchild_indent
      << "\"translation_unit_registration_order_ordinal\": "
      << options.translation_unit_registration_order_ordinal << "\n";
  out << child_indent << "},\n";
}
