#include "artifacts/objc3_frontend_artifact_sanity.h"

#include "ast/objc3_ast_declarations.h"

namespace objc3c::artifacts {

namespace {

std::size_t CountSubstringOccurrences(const std::string &text,
                                      const std::string &needle) {
  if (needle.empty()) {
    return 0u;
  }
  std::size_t count = 0u;
  std::size_t offset = 0u;
  while ((offset = text.find(needle, offset)) != std::string::npos) {
    ++count;
    offset += needle.size();
  }
  return count;
}

bool HasRuntimeBearingExecutableSurface(
    const Objc3Program &program,
    std::size_t message_send_sites) {
  if (message_send_sites > 0u) {
    return true;
  }
  for (const auto &implementation : program.implementations) {
    if (!implementation.properties.empty()) {
      return true;
    }
    for (const auto &method : implementation.methods) {
      if (!method.body.empty()) {
        return true;
      }
    }
  }
  return false;
}

}  // namespace

bool IsSuspiciousObjc3NativeIRTruthGap(
    const std::string &ir_text,
    const Objc3Program &program,
    std::size_t message_send_sites) {
  if (!HasRuntimeBearingExecutableSurface(program, message_send_sites)) {
    return false;
  }

  const bool has_dispatch_surface =
      ir_text.find("@objc3_runtime_dispatch_i32") != std::string::npos;
  const bool has_property_runtime_surface =
      ir_text.find("@objc3_runtime_read_current_property_i32") !=
          std::string::npos ||
      ir_text.find("@objc3_runtime_write_current_property_i32") !=
          std::string::npos ||
      ir_text.find("@objc3_runtime_exchange_current_property_i32") !=
          std::string::npos ||
      ir_text.find("@objc3_runtime_load_weak_current_property_i32") !=
          std::string::npos ||
      ir_text.find("@objc3_runtime_store_weak_current_property_i32") !=
          std::string::npos;
  const bool has_runtime_metadata_surface =
      ir_text.find("__objc3_class_descriptor") != std::string::npos ||
      ir_text.find("__objc3_method_entry") != std::string::npos ||
      ir_text.find("__objc3_property_descriptor") != std::string::npos;

  const std::size_t define_count =
      CountSubstringOccurrences(ir_text, "\ndefine ") +
      (ir_text.rfind("define ", 0) == 0 ? 1u : 0u);
  const bool has_stub_main =
      ir_text.find("define i32 @main()") != std::string::npos &&
      ir_text.find("ret i32 0") != std::string::npos && define_count <= 1u;

  return has_stub_main ||
         (!has_dispatch_surface && !has_property_runtime_surface &&
          !has_runtime_metadata_surface);
}

}  // namespace objc3c::artifacts
