#include "driver/objc3_llvm_capability_routing.h"

#include <filesystem>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "io/objc3_file_io.h"

namespace {

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

bool ExtractObjectSegment(const std::string &text, const std::string &name, std::string &segment) {
  const std::string key = "\"" + name + "\"";
  const std::size_t key_pos = text.find(key);
  if (key_pos == std::string::npos) {
    return false;
  }
  const std::size_t object_start = text.find('{', key_pos);
  if (object_start == std::string::npos) {
    return false;
  }

  int depth = 0;
  for (std::size_t i = object_start; i < text.size(); ++i) {
    const char ch = text[i];
    if (ch == '{') {
      ++depth;
    } else if (ch == '}') {
      --depth;
      if (depth == 0) {
        segment = text.substr(object_start, i - object_start + 1);
        return true;
      }
      if (depth < 0) {
        return false;
      }
    }
  }
  return false;
}

bool ExtractBoolField(const std::string &text, const std::string &name, bool &value) {
  const std::string key = "\"" + name + "\"";
  const std::size_t key_pos = text.find(key);
  if (key_pos == std::string::npos) {
    return false;
  }
  const std::size_t colon_pos = text.find(':', key_pos + key.size());
  if (colon_pos == std::string::npos) {
    return false;
  }
  const std::size_t bool_pos = text.find_first_not_of(" \t\r\n", colon_pos + 1);
  if (bool_pos == std::string::npos) {
    return false;
  }
  if (text.compare(bool_pos, 4, "true") == 0) {
    value = true;
    return true;
  }
  if (text.compare(bool_pos, 5, "false") == 0) {
    value = false;
    return true;
  }
  return false;
}

bool ExtractStringField(const std::string &text, const std::string &name, std::string &value) {
  const std::string key = "\"" + name + "\"";
  const std::size_t key_pos = text.find(key);
  if (key_pos == std::string::npos) {
    return false;
  }
  const std::size_t colon_pos = text.find(':', key_pos + key.size());
  if (colon_pos == std::string::npos) {
    return false;
  }
  const std::size_t quote_start = text.find('"', colon_pos + 1);
  if (quote_start == std::string::npos) {
    return false;
  }
  const std::size_t quote_end = text.find('"', quote_start + 1);
  if (quote_end == std::string::npos) {
    return false;
  }
  value = text.substr(quote_start + 1, quote_end - quote_start - 1);
  return true;
}

std::vector<std::string> ExtractStringArrayField(const std::string &text, const std::string &name) {
  std::vector<std::string> values;
  const std::string key = "\"" + name + "\"";
  const std::size_t key_pos = text.find(key);
  if (key_pos == std::string::npos) {
    return values;
  }
  const std::size_t colon_pos = text.find(':', key_pos + key.size());
  if (colon_pos == std::string::npos) {
    return values;
  }
  const std::size_t array_start = text.find('[', colon_pos + 1);
  if (array_start == std::string::npos) {
    return values;
  }
  const std::size_t array_end = text.find(']', array_start + 1);
  if (array_end == std::string::npos || array_end <= array_start) {
    return values;
  }
  const std::string body = text.substr(array_start + 1, array_end - array_start - 1);

  std::size_t cursor = 0;
  while (cursor < body.size()) {
    const std::size_t quote_start = body.find('"', cursor);
    if (quote_start == std::string::npos) {
      break;
    }
    const std::size_t quote_end = body.find('"', quote_start + 1);
    if (quote_end == std::string::npos) {
      break;
    }
    values.push_back(body.substr(quote_start + 1, quote_end - quote_start - 1));
    cursor = quote_end + 1;
  }
  return values;
}

bool ParseCapabilitySummary(const std::string &text, Objc3LLVMCabilitySummary &summary, std::string &error) {
  if (!ExtractStringField(text, "mode", summary.mode)) {
    error = "llvm capability summary parse failure: missing mode";
    return false;
  }
  if (summary.mode != "objc3c-llvm-capabilities-v2") {
    error = "llvm capability summary mode mismatch: expected objc3c-llvm-capabilities-v2";
    return false;
  }

  std::string clang_segment;
  if (!ExtractObjectSegment(text, "clang", clang_segment) ||
      !ExtractStringField(clang_segment, "path", summary.clang_path) ||
      !ExtractBoolField(clang_segment, "found", summary.clang_found)) {
    error = "llvm capability summary parse failure: invalid clang capability section";
    return false;
  }

  std::string llc_segment;
  if (!ExtractObjectSegment(text, "llc", llc_segment) || !ExtractStringField(llc_segment, "path", summary.llc_path) ||
      !ExtractBoolField(llc_segment, "found", summary.llc_found)) {
    error = "llvm capability summary parse failure: invalid llc capability section";
    return false;
  }

  std::string llc_features_segment;
  if (!ExtractObjectSegment(text, "llc_features", llc_features_segment) ||
      !ExtractBoolField(llc_features_segment, "supports_filetype_obj", summary.llc_supports_filetype_obj)) {
    error = "llvm capability summary parse failure: invalid llc_features section";
    return false;
  }

  std::string sema_segment;
  if (!ExtractObjectSegment(text, "sema_type_system_parity", sema_segment) ||
      !ExtractBoolField(sema_segment, "parity_ready", summary.parity_ready)) {
    error = "llvm capability summary parse failure: invalid sema/type-system parity section";
    return false;
  }
  summary.blockers = ExtractStringArrayField(sema_segment, "blockers");
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

  if (!std::filesystem::exists(options.llvm_capabilities_summary)) {
    error = "capability routing fail-closed: llvm capabilities summary missing: " +
            options.llvm_capabilities_summary.string();
    return false;
  }

  const std::string payload = ReadText(options.llvm_capabilities_summary);
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
