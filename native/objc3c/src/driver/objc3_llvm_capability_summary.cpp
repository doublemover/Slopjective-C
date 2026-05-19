#include "driver/objc3_llvm_capability_summary.h"

#include <exception>
#include <filesystem>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "io/objc3_file_io.h"
#include "io/json/json_parser.h"

namespace {

using objc3::io::json::JsonValue;

std::vector<std::string> ReadStringArrayField(const JsonValue &object,
                                              std::string_view name) {
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

bool ParseObjc3LLVMCapabilitySummary(const std::string &text,
                                     Objc3LLVMCapabilitySummary &summary,
                                     std::string &error) {
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
  if (clang == nullptr || !clang->IsObject() || !clang->GetString("path") ||
      !clang->GetBool("found")) {
    error = "llvm capability summary parse failure: invalid clang capability section";
    return false;
  }
  summary.clang_path = *clang->GetString("path");
  summary.clang_found = *clang->GetBool("found");

  const JsonValue *llc = root.Find("llc");
  if (llc == nullptr || !llc->IsObject() || !llc->GetString("path") ||
      !llc->GetBool("found")) {
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
  summary.llc_supports_filetype_obj =
      *llc_features->GetBool("supports_filetype_obj");

  const JsonValue *sema = root.Find("sema_type_system_parity");
  if (sema == nullptr || !sema->IsObject() || !sema->GetBool("parity_ready")) {
    error = "llvm capability summary parse failure: invalid sema/type-system parity section";
    return false;
  }
  summary.parity_ready = *sema->GetBool("parity_ready");
  summary.blockers = ReadStringArrayField(*sema, "blockers");
  return true;
}

}  // namespace

bool TryLoadObjc3LLVMCapabilitySummary(
    const std::filesystem::path &summary_path,
    Objc3LLVMCapabilitySummary &summary,
    std::string &error) {
  std::error_code exists_error;
  const bool summary_exists = std::filesystem::exists(summary_path, exists_error);
  if (exists_error) {
    error = "unable to stat llvm capabilities summary '" +
            summary_path.string() + "': " + exists_error.message();
    return false;
  }
  if (!summary_exists) {
    error = "llvm capabilities summary missing: " + summary_path.string();
    return false;
  }

  std::string payload;
  try {
    payload = ReadText(summary_path);
  } catch (const std::exception &read_error) {
    error = read_error.what();
    return false;
  }
  return ParseObjc3LLVMCapabilitySummary(payload, summary, error);
}

std::string DescribeObjc3LLVMCapabilityBlockers(
    const std::vector<std::string> &blockers) {
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
