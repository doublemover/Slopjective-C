#include "driver/objc3_llvm_capability_routing.h"

#include <exception>
#include <filesystem>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "io/objc3_file_io.h"
#include "io/json/json_parser.h"

namespace {

using objc3::io::json::JsonValue;

struct Objc3LLVMCabilitySummary {
  std::string mode;
  std::string clang_path;
  bool clang_found = false;
  std::string llc_path;
  bool llc_found = false;
  bool llc_supports_filetype_obj = false;
  bool parity_ready = false;
  std::vector<std::string> blockers;
};

std::vector<std::string> ReadStringArrayField(const JsonValue &object, std::string_view name) {
  std::vector<std::string> values;
  const JsonValue *field = object.Find(name);
  if (field == nullptr || !field->IsArray()) {
    return values;
  }
  for (const JsonValue &entry : field->AsArray()) {
    if (entry.IsString()) {
      values.push_back(entry.AsString());
    }
  }
  return values;
}

bool ParseCapabilitySummary(const std::string &text, Objc3LLVMCabilitySummary &summary, std::string &error) {
  const auto parsed = objc3::io::json::ParseJson(text);
  if (!parsed.ok()) {
    error = "llvm capability summary parse failure: " + parsed.error->Format();
    return false;
  }
  const JsonValue &root = parsed.value;
  if (!root.IsObject()) {
    error = "llvm capability summary parse failure: root must be an object";
    return false;
  }

  const auto mode = root.GetString("mode");
  if (!mode) {
    error = "llvm capability summary parse failure: missing mode";
    return false;
  }
  summary.mode = *mode;
  if (summary.mode != "objc3c-llvm-capabilities-v2") {
    error = "llvm capability summary mode mismatch: expected objc3c-llvm-capabilities-v2";
    return false;
  }

  const JsonValue *clang = root.Find("clang");
  if (clang == nullptr || !clang->IsObject() || !clang->GetString("path") || !clang->GetBool("found")) {
    error = "llvm capability summary parse failure: invalid clang capability section";
    return false;
  }
  summary.clang_path = *clang->GetString("path");
  summary.clang_found = *clang->GetBool("found");

  const JsonValue *llc = root.Find("llc");
  if (llc == nullptr || !llc->IsObject() || !llc->GetString("path") || !llc->GetBool("found")) {
    error = "llvm capability summary parse failure: invalid llc capability section";
    return false;
  }
  summary.llc_path = *llc->GetString("path");
  summary.llc_found = *llc->GetBool("found");

  const JsonValue *llc_features = root.Find("llc_features");
  if (llc_features == nullptr || !llc_features->IsObject() ||
      !llc_features->GetBool("supports_filetype_obj")) {
    error = "llvm capability summary parse failure: invalid llc_features section";
    return false;
  }
  summary.llc_supports_filetype_obj = *llc_features->GetBool("supports_filetype_obj");

  const JsonValue *sema = root.Find("sema_type_system_parity");
  if (sema == nullptr || !sema->IsObject() || !sema->GetBool("parity_ready")) {
    error = "llvm capability summary parse failure: invalid sema/type-system parity section";
    return false;
  }
  summary.parity_ready = *sema->GetBool("parity_ready");
  summary.blockers = ReadStringArrayField(*sema, "blockers");
  return true;
}

std::string JoinBlockers(const std::vector<std::string> &blockers) {
  if (blockers.empty()) {
    return "unspecified";
  }
  std::ostringstream out;
  for (std::size_t i = 0; i < blockers.size(); ++i) {
    if (i > 0) {
      out << ", ";
    }
    out << blockers[i];
  }
  return out.str();
}

}  // namespace

bool ApplyObjc3LLVMCabilityRouting(Objc3CliOptions &options, std::string &error) {
  if (options.llvm_capabilities_summary.empty()) {
    if (options.route_backend_from_capabilities) {
      error = "capability routing fail-closed: --objc3-route-backend-from-capabilities requires --llvm-capabilities-summary";
      return false;
    }
    return true;
  }

  std::error_code exists_error;
  const bool summary_exists = std::filesystem::exists(options.llvm_capabilities_summary, exists_error);
  if (exists_error) {
    error = "capability routing fail-closed: unable to stat llvm capabilities summary '" +
            options.llvm_capabilities_summary.string() + "': " + exists_error.message();
    return false;
  }
  if (!summary_exists) {
    error = "capability routing fail-closed: llvm capabilities summary missing: " +
            options.llvm_capabilities_summary.string();
    return false;
  }

  std::string payload;
  try {
    payload = ReadText(options.llvm_capabilities_summary);
  } catch (const std::exception &read_error) {
    error = "capability routing fail-closed: " + std::string(read_error.what());
    return false;
  }
  Objc3LLVMCabilitySummary summary;
  if (!ParseCapabilitySummary(payload, summary, error)) {
    error = "capability routing fail-closed: " + error;
    return false;
  }

  if (!summary.parity_ready) {
    error = "capability routing fail-closed: sema/type-system parity capability unavailable: " +
            JoinBlockers(summary.blockers);
    return false;
  }

  if (!options.clang_path_explicit && !summary.clang_path.empty()) {
    options.clang_path = summary.clang_path;
  }
  if (!options.llc_path_explicit && !summary.llc_path.empty()) {
    options.llc_path = summary.llc_path;
  }

  if (options.route_backend_from_capabilities) {
    options.ir_object_backend =
        summary.llc_supports_filetype_obj ? Objc3IrObjectBackend::kLLVMDirect : Objc3IrObjectBackend::kClang;
  }

  if (options.ir_object_backend == Objc3IrObjectBackend::kClang && !summary.clang_found) {
    error = "capability routing fail-closed: clang backend selected but capability summary reports clang unavailable";
    return false;
  }
  if (options.ir_object_backend == Objc3IrObjectBackend::kLLVMDirect &&
      (!summary.llc_found || !summary.llc_supports_filetype_obj)) {
    error =
        "capability routing fail-closed: llvm-direct backend selected but llc --filetype=obj capability is unavailable";
    return false;
  }
  return true;
}
