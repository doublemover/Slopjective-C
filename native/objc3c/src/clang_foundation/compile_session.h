#pragma once

#include <optional>
#include <string>
#include <vector>

#include "clang_foundation/interface_contract.h"

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
  std::vector<std::string> driver_arguments;
  std::vector<CompileDiagnostic> diagnostics;
  std::vector<InterfaceDeclarationIdentity> declarations;
  std::string target_triple;
  std::string runtime_abi;
  std::optional<InterfaceEmissionArtifact> interface_artifact;
};

// Both product entry points own one request and use this in-process frontend.
// Clang/LLVM private types never cross the embedding ABI.
CompileResult Compile(const CompileRequest &request);

}  // namespace objc3::clang_foundation
