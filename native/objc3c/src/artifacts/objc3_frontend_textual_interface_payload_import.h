#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace objc3::artifacts::frontend {

inline constexpr const char *kObjc3StandaloneTextualInterfaceImportDiagnostic =
    "O3IFC8238";

struct Objc3StandaloneTextualInterfaceImportDiagnostic {
  std::string code;
  std::string message;
  std::string json_path;
};

struct Objc3StandaloneTextualInterfaceImportResult {
  bool parse_status_supported = false;
  bool semantic_equivalence_supported = false;
  std::string payload_id;
  std::string module_id;
  std::string package_id;
  std::string replay_key;
  std::size_t import_count = 0;
  std::size_t declaration_count = 0;
  std::size_t reserved_metadata_count = 0;
  std::size_t diagnostic_count = 0;
  std::vector<Objc3StandaloneTextualInterfaceImportDiagnostic> diagnostics;

  [[nodiscard]] bool ok() const {
    return parse_status_supported && semantic_equivalence_supported &&
           diagnostics.empty();
  }
};

[[nodiscard]] Objc3StandaloneTextualInterfaceImportResult
ValidateObjc3StandaloneTextualInterfacePayloadImport(
    std::string_view payload_json);

}  // namespace objc3::artifacts::frontend
