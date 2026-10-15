#pragma once

#include <optional>
#include <string>
#include <vector>

#include "clang_foundation/interface_contract.h"
#include "clang_foundation/interface_import.h"
#include "clang_foundation/interface_module_resolver.h"

namespace objc3::clang_foundation {

enum class BaseLanguage { C, Cxx, ObjectiveC, ObjectiveCxx };
enum class EmitKind { Syntax, Object, LLVM };
enum class SemanticProfile { Compatibility, Strict, StrictConcurrency, StrictSystem };

struct CompileRequest {
  std::string input_path;
  std::optional<std::string> source_text;
  std::string working_directory;
  std::string output_path;
  BaseLanguage language = BaseLanguage::ObjectiveC;
  SemanticProfile profile = SemanticProfile::Compatibility;
  EmitKind emit = EmitKind::Syntax;
  bool objc3 = false;
  bool arc = false;
  bool strict_concurrency = false;
  bool emit_semantic_interface = false;
  std::string target_triple;
  std::string objc_runtime;
  std::vector<std::string> arguments;
  std::string module_name;
  std::string package_identity;
  // Optional publication destination. With emitted object/IR output, all
  // products are published through one rollback boundary.
  std::string interface_output_path;
  // Internal CLI publication destination. The C embedding API continues to
  // return this record in memory. Disk publication shares the product rollback
  // boundary, including failures after a record has been staged.
  std::string dependency_record_output_path;
  std::vector<std::string> interface_search_paths;
  // Locked modules available to source-level `@import`. When enabled, these
  // are prepared before parsing but remain invisible until the matching
  // source directive is encountered.
  bool source_directed_imports = false;
  std::vector<InterfaceModuleRequest> module_imports;
  std::vector<InterfaceImportInput> imported_interfaces;
};

struct DiagnosticLocation {
  std::string file;
  unsigned line = 0;
  unsigned column = 0;
};

struct CompileDiagnostic {
  unsigned id = 0;
  unsigned severity = 0;
  std::string message;
  DiagnosticLocation spelling;
  DiagnosticLocation expansion;
};

struct CompileResult {
  bool success = false;
  std::string failure;
  std::string compiler_identity;
  std::string compiler_driver_path;
  std::string compiler_driver_sha256;
  std::string resource_directory;
  std::string resource_directory_sha256;
  std::string clang_patch_digest;
  std::vector<std::string> driver_arguments;
  std::vector<CompileDiagnostic> diagnostics;
  std::vector<InterfaceDeclarationIdentity> declarations;
  std::vector<InterfaceDeclarationIdentity> imported_declarations;
  std::vector<InterfaceDependency> resolved_interface_dependencies;
  std::vector<std::string> interface_dependency_order;
  std::vector<InterfaceModuleIdentity> resolved_module_identities;
  std::vector<InterfaceSourceArtifact> resolved_interface_sources;
  // Physical interface inputs prepared by the native import session. Keep
  // publication destinations from replacing those provider artifacts.
  std::vector<std::string> interface_input_paths;
  std::vector<std::string> visible_module_names;
  std::string semantic_dependency_record;
  std::vector<InterfaceContractDiagnostic> interface_import_diagnostics;
  std::string target_triple;
  std::string runtime_abi;
  std::optional<InterfaceEmissionArtifact> interface_artifact;
};

// Both product entry points own one request and use this in-process frontend.
// Clang/LLVM private types never cross the embedding ABI.
CompileResult Compile(const CompileRequest &request);

}  // namespace objc3::clang_foundation
